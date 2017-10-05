/*************************************************************************
This file is part of libresourceqt

Copyright (C) 2011 Nokia Corporation.

This library is free software; you can redistribute
it and/or modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation
version 2.1 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
USA.
*************************************************************************/

#include "resource-engine.h"
#include <dbus/dbus.h>

using namespace ResourcePolicy;

static QMultiMap<resconn_t *, ResourceEngine *> engineMap;

resconn_t *ResourceEngine::libresourceConnection = NULL;
quint32 ResourceEngine::libresourceUsers = 0;

static QMutex mutex(QMutex::Recursive);

static inline quint32 allResourcesToBitmask(const ResourceSet *resourceSet);
static inline quint32 optionalResourcesToBitmask(const ResourceSet *resourceSet);

static void connectionIsUp(resconn_t *connection);
static void statusCallbackHandler(resset_t *rset, resmsg_t *msg);
static void handleUnregisterMessage(resmsg_t *, resset_t *, void *data);
static void handleGrantMessage(resmsg_t *msg, resset_t *rs, void *data);
static void handleAdviceMessage(resmsg_t *msg, resset_t *rs, void *data);
static void handleReleaseMessage(resmsg_t *message, resset_t *rs, void *data);

extern bool printLogs;

ResourceEngine::ResourceEngine(ResourceSet *resourceSet)
    : QObject(), connected(false), resourceSet(resourceSet),
      libresourceSet(NULL), requestId(0), messageMap(), connectionMode(0),
      identifier(resourceSet->id()), aboutToBeDeleted(false), isConnecting(false)
{
    //if (resourceSet->alwaysGetReply()) {
        connectionMode += RESMSG_MODE_ALWAYS_REPLY;
    //}
    if (resourceSet->willAutoRelease()) {
        connectionMode += RESOURCE_AUTO_RELEASE;
    }
    LOG_DEBUG("ResourceEngine::ResourceEngine(%d) - connectionMode = %04x", identifier, connectionMode);
}

ResourceEngine::~ResourceEngine()
{
    LOG_DEBUG("ResourceEngine(%d)::%s() - **************** locking....", identifier, __FUNCTION__);
    QMutexLocker locker(&mutex);
    LOG_DEBUG("ResourceEngine::~ResourceEngine(%d) - starting destruction", identifier);
    libresourceUsers--;
    if (libresourceSet != NULL) {
        libresourceSet->userdata = NULL;
        LOG_DEBUG("ResourceEngine::~ResourceEngine(%d) - unset userdata", identifier);
    }
    if (libresourceUsers == 0) {
        // Let's just print a log message and still keep
        // ResourceEngine::libresourceConnection around in case we get new
        // users (previously it was set to null, effectively leaking the
        // d-bus socket fd). We'll still leak the socket fd when the client
        // terminates though.
        LOG_DEBUG("ResourceEngine::~ResourceEngine(%d) - last libresourceUser!", identifier);
    }
    LOG_DEBUG("ResourceEngine::~ResourceEngine(%d) is no more!", identifier);
}

bool ResourceEngine::initialize()
{
    LOG_DEBUG("ResourceEngine(%d)::%s() - **************** locking....", identifier, __FUNCTION__);
    QMutexLocker locker(&mutex);
    DBusError dbusError;
    DBusConnection *dbusConnection;

    if (ResourceEngine::libresourceConnection == NULL) {
        dbus_error_init(&dbusError);
        dbusConnection = dbus_bus_get_private(DBUS_BUS_SYSTEM, &dbusError);
        if (dbus_error_is_set(&dbusError)) {
            LOG_DEBUG("Error getting the system bus: %s", dbusError.message);
            dbus_error_free(&dbusError);
            return false;
        }
        dbus_error_free(&dbusError);
        DBUSConnectionEventLoop::addConnection(dbusConnection);

        ResourceEngine::libresourceConnection = resproto_init(RESPROTO_ROLE_CLIENT, RESPROTO_TRANSPORT_DBUS,
                                              connectionIsUp, dbusConnection);
        if (ResourceEngine::libresourceConnection == NULL) {
            LOG_DEBUG("resproto_init failed!");
            return false;
        }
        ResourceEngine::libresourceUsers = 1;
        resproto_set_handler(ResourceEngine::libresourceConnection, RESMSG_UNREGISTER, handleUnregisterMessage);
        resproto_set_handler(ResourceEngine::libresourceConnection, RESMSG_GRANT, handleGrantMessage);
        resproto_set_handler(ResourceEngine::libresourceConnection, RESMSG_ADVICE, handleAdviceMessage);
        resproto_set_handler(ResourceEngine::libresourceConnection, RESMSG_RELEASE, handleReleaseMessage);
        engineMap.insert(ResourceEngine::libresourceConnection, this);
    } else {
        ResourceEngine::libresourceUsers += 1;
        engineMap.insert(ResourceEngine::libresourceConnection, this);
    }

    LOG_DEBUG("ResourceEngine (%u, %p) is now initialized. %d users",
           identifier, ResourceEngine::libresourceConnection,
           ResourceEngine::libresourceUsers);
    return true;
}

static void handleUnregisterMessage(resmsg_t *message, resset_t *libresourceSet, void *)
{
    LOG_DEBUG("**************** %s() - locking....", __FUNCTION__);
    QMutexLocker locker(&mutex);
    if (NULL == libresourceSet->userdata) {
        LOG_DEBUG("IGNORING unregister, no context");
        return;
    }
    ResourceEngine *engine = reinterpret_cast<ResourceEngine *>(libresourceSet->userdata);
    LOG_DEBUG("recv: unregister: id=%d, engine->id() = %d", message->any.id, engine->id());
    if (engine->id() != message->any.id) {
        LOG_DEBUG("Received an unregister notification, but it is not for us. Ignoring (%d != %d)",
               message->any.id, engine->id());
        return;
    }

    engine->disconnected();
}

void ResourceEngine::disconnected()
{
    LOG_DEBUG("ResourceEngine(%d) - disconnected", identifier);
    connected = false;
    emit disconnectedFromManager();
}

static void handleGrantMessage(resmsg_t *message, resset_t *libresourceSet, void *)
{
    LOG_DEBUG("**************** %s() - locking....", __FUNCTION__);
    QMutexLocker locker(&mutex);
    if (NULL == libresourceSet->userdata) {
        LOG_DEBUG("IGNORING grant, no context: type=0x%04x, id=0x%04x, reqno=0x%04x, resc=0x%04x",
               message->notify.type, message->notify.id, message->notify.reqno, message->notify.resrc);
        return;
    }
    ResourceEngine *engine = reinterpret_cast<ResourceEngine *>(libresourceSet->userdata);
    LOG_DEBUG("recv: grant: type=%d, id=%d, reqno=%d, resc=0x%04x engine->id() = %d",
           message->notify.type, message->notify.id, message->notify.reqno,
           message->notify.resrc, engine->id());
    if (engine->id() != message->any.id) {
        LOG_DEBUG("Received a grant message, but it is not for us. Ignoring (%d != %d)",
               engine->id(), message->any.id);
        return;
    }
    engine->receivedGrant(&(message->notify));
}

void ResourceEngine::receivedGrant(resmsg_notify_t *notifyMessage)
{
    LOG_DEBUG("ResourceEngine(%d) -- receivedGrant: type=0x%04x, id=0x%04x, reqno=0x%04x, resc=0x%04x",
           identifier, notifyMessage->type, notifyMessage->id, notifyMessage->reqno, notifyMessage->resrc);

    if (notifyMessage->resrc == 0) {

        bool unkownRequest                = !messageMap.contains(notifyMessage->reqno);
        resmsg_type_t originalMessageType =  messageMap.take(notifyMessage->reqno);

        LOG_DEBUG("ResourceEngine(%d) -- originalMessageType=%u", identifier, originalMessageType);

        if (unkownRequest) {
            //we don't know this req number => it must be a server override
            LOG_DEBUG("ResourceEngine(%d) -- emiting signal resourcesLost()", identifier);
            emit resourcesLost(allResourcesToBitmask(resourceSet));

        } else if (originalMessageType == RESMSG_UPDATE) {
            //An app can loose all resources with update() or if it had no resources,
            //it can be ACKed saying that the update() was OK but you have no resources yet.

            if (resourceSet->hasResourcesGranted()) {
                LOG_DEBUG("ResourceEngine(%d) -- emitting signal resourcesLost() for update", identifier);
                emit resourcesLost(allResourcesToBitmask(resourceSet));
            } else {
                if ( resourceSet->alwaysGetReply() ) {
                    //If alwaysReply is on and we didn't have resources at update() then we come from here to updateOK()
                    LOG_DEBUG("ResourceEngine(%d) -- emitting signal updateOK() via receivedGrant.", identifier);
                    emit updateOK(true);
                } else {
                    emit updateOK(false);
                }
            }

        } else if (originalMessageType == RESMSG_ACQUIRE && resourceSet->alwaysGetReply() ) {
            LOG_DEBUG("ResourceEngine(%d) -- request DENIED!", identifier);
            emit resourcesDenied();
        } else if (originalMessageType == RESMSG_RELEASE) {
            LOG_DEBUG("ResourceEngine(%d) -- confirmation to release", identifier);
            emit resourcesReleased();
        } else {
            LOG_DEBUG("ResourceEngine(%d) -- Ignoring the receivedGrant because original message unknown.", identifier);
        }
    } else {

        LOG_DEBUG("ResourceEngine(%d) - emitting signal resourcesGranted(%02x).", identifier, notifyMessage->resrc);
        emit resourcesGranted(notifyMessage->resrc);
    }

    messageMap.remove(notifyMessage->reqno);
}


static void handleReleaseMessage(resmsg_t *message, resset_t *rs, void *)
{
    LOG_DEBUG("**************** %s() - locking....", __FUNCTION__);
    QMutexLocker locker(&mutex);
    if (NULL == rs->userdata) {
        LOG_DEBUG("IGNORING release, no context");
        return;
    }
    ResourceEngine *engine = reinterpret_cast<ResourceEngine *>(rs->userdata);
    LOG_DEBUG("recv: release: type=%d, id=%d, reqno=%d, resc=0x%04x engine->id() = %d",
           message->notify.type, message->notify.id, message->notify.reqno,
           message->notify.resrc, engine->id());
    if (engine->id() != message->any.id) {
        LOG_DEBUG("Received an advice message, but it is not for us. Ignoring (%d != %d)",
               engine->id(), message->any.id);
        return;
    }

    engine->receivedRelease(&(message->notify));
}

void ResourceEngine::receivedRelease(resmsg_notify_t *message)
{
    uint32_t allResources = allResourcesToBitmask(resourceSet);
    LOG_DEBUG("ResourceEngine(%d) - %s: have: %02x got %02x", identifier, __FUNCTION__, allResources, message->resrc);
    emit resourcesReleasedByManager();
}

static void handleAdviceMessage(resmsg_t *message, resset_t *libresourceSet, void *)
{
    LOG_DEBUG("**************** %s() - locking....", __FUNCTION__);
    QMutexLocker locker(&mutex);
    if (NULL == libresourceSet->userdata) {
        LOG_DEBUG("IGNORING advice, no context");
        return;
    }
    ResourceEngine *engine = reinterpret_cast<ResourceEngine *>(libresourceSet->userdata);
    LOG_DEBUG("recv: advice: type=%d, id=%d, reqno=%d, resc=0x%04x engine->id() = %d",
           message->notify.type, message->notify.id, message->notify.reqno,
           message->notify.resrc, engine->id());
    if (engine->id() != message->any.id) {
        LOG_DEBUG("Received an advice message, but it is not for us. Ignoring (%d != %d)",
               engine->id(), message->any.id);
        return;
    }

    engine->receivedAdvice(&(message->notify));
}

void ResourceEngine::receivedAdvice(resmsg_notify_t *message)
{
    uint32_t allResources = allResourcesToBitmask(resourceSet);
    LOG_DEBUG("ResourceEngine(%d) - %s: have: %02x got %02x", identifier, __FUNCTION__, allResources, message->resrc);
    emit resourcesBecameAvailable(message->resrc);
}

bool ResourceEngine::connectToManager()
{
    LOG_DEBUG("ResourceEngine(%d)::%s() - **************** locking....", identifier, __FUNCTION__);
    QMutexLocker locker(&mutex);
    if (isConnecting) {
        LOG_DEBUG("ResourceEngine::%s().... allready connecting, ignoring request", __FUNCTION__);
        return true;
    }
    isConnecting = true;
    resmsg_t resourceMessage;
    memset(&resourceMessage, 0, sizeof(resmsg_t));
    resourceMessage.record.type = RESMSG_REGISTER;
    resourceMessage.record.id = resourceSet->id();
    resourceMessage.record.reqno = ++requestId;

    messageMap.insert(requestId, RESMSG_REGISTER);

    uint32_t allResources, optionalResources;
    allResources = allResourcesToBitmask(resourceSet);
    optionalResources = optionalResourcesToBitmask(resourceSet);

    resourceMessage.record.rset.all = allResources;
    resourceMessage.record.rset.opt = optionalResources;
    resourceMessage.record.rset.share = 0;
    resourceMessage.record.rset.mask = 0;

    QByteArray ba = resourceSet->applicationClass().toLatin1();
    resourceMessage.record.klass = ba.data();

    resourceMessage.record.mode = connectionMode;

    LOG_DEBUG("ResourceEngine(%d) - ResourceEngine is now connecting(%d, %d, %d)",
           identifier, resourceMessage.record.id, resourceMessage.record.reqno,
           resourceMessage.record.rset.all);
    libresourceSet = resconn_connect(ResourceEngine::libresourceConnection, &resourceMessage,
                                     statusCallbackHandler);
    if (libresourceSet == NULL)
        return false;
    libresourceSet->userdata = this; //save our context
    //locker.unlock();
    LOG_DEBUG("ResourceEngine(%d)::%s() - **************** unlocked! returning true", identifier, __FUNCTION__);
    return true;
}

bool ResourceEngine::disconnectFromManager()
{
    LOG_DEBUG("ResourceEngine(%d)::%s() - **************** locking....", identifier, __FUNCTION__);
    QMutexLocker locker(&mutex);
    resmsg_t resourceMessage;
    memset(&resourceMessage, 0, sizeof(resmsg_t));

    LOG_DEBUG("ResourceEngine(%d)::%s() - disconnecting from manager - %p",
           identifier, __FUNCTION__, ResourceEngine::libresourceConnection);
    connected = false;
    aboutToBeDeleted = true;

    resourceMessage.record.type = RESMSG_UNREGISTER;
    resourceMessage.record.id = resourceSet->id();
    resourceMessage.record.reqno = ++requestId;

//    messageMap.insert(requestId, RESMSG_UNREGISTER);

    bool ret = true;
    if (libresourceSet != NULL) {
        ret = resconn_disconnect(libresourceSet, &resourceMessage, statusCallbackHandler)?true:false;
    }
    return ret;
}

bool ResourceEngine::toBeDeleted()
{
    return aboutToBeDeleted;
}

static inline quint32 allResourcesToBitmask(const ResourceSet *resourceSet)
{
    QList<Resource *> resourceList = resourceSet->resources();
    quint32 bitmask = 0;
    for (int i = 0; i < resourceList.size(); i++) {
        quint32 bits = resourceTypeToLibresourceType(resourceList[i]->type());
        LOG_DEBUG("Converted Resource 0x%02x to 0x%02x", resourceList[i]->type(), bits);
        bitmask += bits;
    }
    LOG_DEBUG("All resources as bitmask is 0x%04x", bitmask);
    return bitmask;
}

quint32 ResourcePolicy::resourceTypeToLibresourceType(ResourceType type)
{
    switch (type) {
    case AudioPlaybackType:
        return RESMSG_AUDIO_PLAYBACK;
    case VideoPlaybackType:
        return RESMSG_VIDEO_PLAYBACK;
    case AudioRecorderType:
        return RESMSG_AUDIO_RECORDING;
    case VideoRecorderType:
        return RESMSG_VIDEO_RECORDING;
    case VibraType:
        return RESMSG_VIBRA;
    case LedsType:
        return RESMSG_LEDS;
    case BacklightType:
        return RESMSG_BACKLIGHT;
    case SystemButtonType:
        return RESMSG_SYSTEM_BUTTON;
    case LockButtonType:
        return RESMSG_LOCK_BUTTON;
    case ScaleButtonType:
        return RESMSG_SCALE_BUTTON;
    case SnapButtonType:
        return RESMSG_SNAP_BUTTON;
    case LensCoverType:
        return RESMSG_LENS_COVER;
    case HeadsetButtonsType:
        return RESMSG_HEADSET_BUTTONS;
    case RearFlashlightType:
        return RESMSG_REAR_FLASHLIGHT;
    default:
        LOG_DEBUG("Unknown resource Type %d", quint32(type));
        return 0xffff;
    }
}

static inline quint32 optionalResourcesToBitmask(const ResourceSet *resourceSet)
{
    QList<Resource *> resourceList = resourceSet->resources();
    quint32 bitmask = 0;
    for (int i = 0; i < resourceList.size(); i++) {
        if (resourceList[i]->isOptional()) {
            bitmask += resourceTypeToLibresourceType(resourceList[i]->type());
        }
    }
    return bitmask;
}

static void statusCallbackHandler(resset_t *libresourceSet, resmsg_t *message)
{
    LOG_DEBUG("**************** %s().... %d", __FUNCTION__, __LINE__);
    //LOG_DEBUG("**************** %s() - locking....", __FUNCTION__);
    QMutexLocker locker(&mutex);
    if (NULL == libresourceSet->userdata) {
        LOG_DEBUG("IGNORING status message, no context: type=0x%04x, id=0x%04x, reqno=0x%04x, errcod=%d",
               message->status.type, message->status.id, message->status.reqno, message->status.errcod);
        return;
    }
    LOG_DEBUG("**************** %s().... %d", __FUNCTION__, __LINE__);
    ResourceEngine *resourceEngine = reinterpret_cast<ResourceEngine *>(libresourceSet->userdata);
    LOG_DEBUG("**************** %s().... %d", __FUNCTION__, __LINE__);
    LOG_DEBUG("**************** %s().... %d", __FUNCTION__, __LINE__);
    LOG_DEBUG("recv: status: id=%d, engine->id() = %d", message->any.id, resourceEngine->id());
    LOG_DEBUG("**************** %s().... %d", __FUNCTION__, __LINE__);
    if (resourceEngine->id() != libresourceSet->id) {
        LOG_DEBUG("Received a status notification, but it is not for us. Ignoring (%d != %d)",
               resourceEngine->id(), libresourceSet->id);
        return;
    }
    LOG_DEBUG("Received a status notification");
    if (message->type != RESMSG_STATUS) {
        LOG_DEBUG("Invalid message type.. (got %x, expected %x", message->type, RESMSG_STATUS);
        return;
    }
    if (message->status.errcod) {
        resourceEngine->handleError(message->status.reqno, message->status.errcod, message->status.errmsg);
    }
    else {
        LOG_DEBUG("Received a status message with id %02x and #:%u", message->status.id, message->status.reqno);
        if (!resourceEngine->isConnectedToManager() && resourceEngine->toBeDeleted()) {
            LOG_DEBUG("%s(%d) - delete resourceEngine %p", __FUNCTION__, __LINE__, resourceEngine);
            delete resourceEngine;
        } else {
            resourceEngine->handleStatusMessage(message->status.reqno);
        }
    }
}

void ResourceEngine::handleStatusMessage(quint32 requestNo)
{
    resmsg_type_t originalMessageType = messageMap.value(requestNo);
    LOG_DEBUG("Received a status message: %u(0x%02x)", requestNo, originalMessageType);
    if (originalMessageType == RESMSG_REGISTER) {
        LOG_DEBUG("ResourceEngine(%d) - connected!", identifier);
        connected = true;
        isConnecting = false;
        emit connectedToManager();
        messageMap.remove(requestNo);
    } else if (originalMessageType == RESMSG_UNREGISTER) {
        LOG_DEBUG("ResourceEngine(%d) - disconnected!", identifier);
        connected = false;
        emit disconnectedFromManager();
        messageMap.remove(requestNo);
    } else if (originalMessageType == RESMSG_UPDATE) {
        LOG_DEBUG("ResourceEngine(%d) - Update status", identifier);
        //We only come here if status ok.

        //bool hadGrantsWhenSentUpdate = false;

        //if ( wasInAcquireMode.contains(requestNo) )
        //   hadGrantsWhenSentUpdate = wasInAcquireMode.take(requestNo);

        //if ( !hadGrantsWhenSentUpdate  &&  !resourceSet->alwaysGetReply() ) {

            //If alwaysReply is off and we didn't have resources at update() emit from here to
            //updateOK() (i.e. ACK that the set we are interested in is changed). Or if alwayReply
            // is off and our update does not change the granted set.
            LOG_DEBUG("ResourceEngine(%d) -- handleStatusMessage.", identifier);
            emit updateOK(false);
        //}

    } else if (originalMessageType == RESMSG_ACQUIRE) {
        LOG_DEBUG("ResourceEngine(%d) - Acquire status", identifier);
    } else if (originalMessageType == RESMSG_RELEASE) {
        LOG_DEBUG("ResourceEngine(%d) - Release status", identifier);
    } else {
        messageMap.remove(requestNo);
    }
}

void ResourceEngine::handleError(quint32 requestNo, qint32 code, const char *message)
{
    resmsg_type_t originalMessageType = messageMap.take(requestNo);
    LOG_DEBUG("ResourceEngine(%d) - Error on request %u(0x%02x): %d - %s",
              identifier, requestNo, originalMessageType, code, message);
    messageMap.remove(requestNo);

    LOG_DEBUG("emitting errorCallback");
    emit errorCallback(code, message);
}

bool ResourceEngine::isConnectedToManager()
{
    return connected;
}

bool ResourceEngine::isConnectingToManager()
{
    return isConnecting;
}

bool ResourceEngine::acquireResources()
{
    LOG_DEBUG("ResourceEngine(%d)::%s() - **************** locking....", identifier, __FUNCTION__);
    QMutexLocker locker(&mutex);
    resmsg_t message;
    memset(&message, 0, sizeof(resmsg_t));

    message.possess.type = RESMSG_ACQUIRE;
    message.possess.id    = resourceSet->id();
    message.possess.reqno = ++requestId;

    messageMap.insert(requestId, RESMSG_ACQUIRE);

    LOG_DEBUG("ResourceEngine(%d) - acquire %u:%u", identifier, resourceSet->id(), requestId);
    int success = resproto_send_message(libresourceSet, &message, statusCallbackHandler);

    return success;
}

bool ResourceEngine::releaseResources()
{
    LOG_DEBUG("ResourceEngine(%d)::%s() - **************** locking....", identifier, __FUNCTION__);
    QMutexLocker locker(&mutex);
    resmsg_t message;
    memset(&message, 0, sizeof(resmsg_t));

    message.possess.type = RESMSG_RELEASE;
    message.possess.id    = resourceSet->id();
    message.possess.reqno = ++requestId;

    messageMap.insert(requestId, RESMSG_RELEASE);
    LOG_DEBUG("ResourceEngine(%d) - release %u:%u", identifier, resourceSet->id(), requestId);
    int success = resproto_send_message(libresourceSet, &message, statusCallbackHandler);

    return success;
}

bool ResourceEngine::updateResources()
{
    LOG_DEBUG("ResourceEngine(%d)::%s() - **************** locking....", identifier, __FUNCTION__);
    QMutexLocker locker(&mutex);
    resmsg_t message;
    memset(&message, 0, sizeof(resmsg_t));
    message.record.type = RESMSG_UPDATE;
    message.record.id = resourceSet->id();
    message.record.reqno = ++requestId;

    uint32_t allResources, optionalResources;
    allResources = allResourcesToBitmask(resourceSet);
    optionalResources = optionalResourcesToBitmask(resourceSet);

    message.record.rset.all = allResources;
    message.record.rset.opt = optionalResources;
    message.record.rset.share = 0;
    message.record.rset.mask = 0;

    QByteArray ba = resourceSet->applicationClass().toLatin1();
    message.record.klass = ba.data();

    messageMap.insert(requestId, RESMSG_UPDATE);

    bool hasGranted = resourceSet->resources().size() ? true : false;

    wasInAcquireMode.insert(requestId, hasGranted /*hasResourcesGranted()*/ );

    LOG_DEBUG("ResourceEngine(%d) - update %u:%u", identifier, resourceSet->id(), requestId);
    int success = resproto_send_message(libresourceSet, &message, statusCallbackHandler);

    return success;
}

bool ResourceEngine::registerAudioProperties(const QString &audioGroup, quint32 pid,
                                              const QString &name, const QString &value)
{
    LOG_DEBUG("ResourceEngine(%d)::%s() - **************** locking....", identifier, __FUNCTION__);
    QMutexLocker locker(&mutex);
    resmsg_t message;
    memset(&message, 0, sizeof(resmsg_t));
    QByteArray groupBa, nameBa, valueBa;

    if (pid != 0) {
        message.audio.pid = pid;
        LOG_DEBUG("ResourceEngine(%d) - audio pid %u", identifier, pid);
    }
    if (!audioGroup.isEmpty() && !audioGroup.isNull()) {
        groupBa = audioGroup.toLatin1();
        message.audio.group = groupBa.data();
        LOG_DEBUG("ResourceEngine(%d) - audio group: %s", identifier, message.audio.group);
    }
    if (!name.isEmpty() && !name.isNull() && !value.isEmpty() && !value.isNull()) {
        nameBa = name.toLatin1();
        valueBa = value.toLatin1();
        message.audio.property.name = nameBa.data();
        message.audio.property.match.method  = resmsg_method_equals;
        message.audio.property.match.pattern = valueBa.data();
        LOG_DEBUG("ResourceEngine(%d) - audio stream tag is %s:%s",
                  identifier, message.audio.property.name, message.audio.property.match.pattern);
    }

    message.audio.type = RESMSG_AUDIO;
    message.audio.id    = resourceSet->id();
    message.audio.reqno = ++requestId;

    message.audio.type  = RESMSG_AUDIO;

    messageMap.insert(requestId, RESMSG_AUDIO);

    LOG_DEBUG("ResourceEngine(%d) - audio %u:%u", identifier, resourceSet->id(), requestId);
    int success = resproto_send_message(libresourceSet, &message, statusCallbackHandler);
    LOG_DEBUG("ResourceEngine(%d) - resproto_send_message returned %d", identifier, success);

    return success;
}

bool ResourceEngine::registerVideoProperties(quint32 pid)
{
    LOG_DEBUG("ResourceEngine(%d)::%s() - **************** locking....", identifier, __FUNCTION__);
    QMutexLocker locker(&mutex);
    resmsg_t message;
    memset(&message, 0, sizeof(resmsg_t));

    if (pid <= 0) {
        LOG_DEBUG("ResourceEngine(%d) - erroneous pid %u", identifier, pid);
        return false;
    }

    message.video.pid   = pid;
    message.video.type  = RESMSG_VIDEO;
    message.video.id    = resourceSet->id();
    message.video.reqno = ++requestId;
    message.video.type  = RESMSG_VIDEO;

    messageMap.insert(requestId, RESMSG_VIDEO);

    LOG_DEBUG("ResourceEngine(%d) - video %u:%u", identifier, resourceSet->id(), requestId);
    int success = resproto_send_message(libresourceSet, &message, statusCallbackHandler);
    LOG_DEBUG("ResourceEngine(%d) - resproto_send_message returned %d", identifier, success);

    return success;
}

static void connectionIsUp(resconn_t *connection)
{
    LOG_DEBUG("**************** %s() - locking....", __FUNCTION__);
    QMutexLocker locker(&mutex);

    LOG_DEBUG("connection is up");

    QList<ResourceEngine*> engines = engineMap.values(connection);
    for (int i = 0; i < engines.size(); ++i) {
        ResourceEngine *resourceEngine = engines.at(i);
        resourceEngine->handleConnectionIsUp(connection);
    }
}

void ResourceEngine::handleConnectionIsUp(resconn_t *connection)
{

    if (ResourceEngine::libresourceConnection == connection) {
        LOG_DEBUG("ResourceEngine(%d) - connected to manager, connection=%p", identifier, connection);
        emit connectedToManager();
    } else {
        LOG_DEBUG("ResourceEngine(%d) - ignoring Connection is up, it is not for us (%p != %p)",
                  identifier, ResourceEngine::libresourceConnection, connection);
    }
}

quint32 ResourceEngine::id()
{
    return identifier;
}
