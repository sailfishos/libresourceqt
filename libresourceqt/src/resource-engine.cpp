/*************************************************************************
This file is part of libresourceqt

Copyright (C) 2010 Nokia Corporation.

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
    qDebug("ResourceEngine::ResourceEngine(%d) - connectionMode = %04x", identifier, connectionMode);
}

ResourceEngine::~ResourceEngine()
{
    qDebug("ResourceEngine(%d)::%s() - **************** locking....", identifier, __FUNCTION__);
    QMutexLocker locker(&mutex);
    qDebug("ResourceEngine::~ResourceEngine(%d) - starting destruction", identifier);
    libresourceUsers--;
    if (libresourceSet != NULL) {
        libresourceSet->userdata = NULL;
        qDebug("ResourceEngine::~ResourceEngine(%d) - unset userdata", identifier);
    }
    if (libresourceUsers==0) {

        qDebug("ResourceEngine::~ResourceEngine(%d) - last libresourceUser!", identifier);
        ResourceEngine::libresourceConnection = NULL;
    }
    qDebug("ResourceEngine::~ResourceEngine(%d) is no more!", identifier);
}
	
bool ResourceEngine::initialize()
{
    qDebug("ResourceEngine(%d)::%s() - **************** locking....", identifier, __FUNCTION__);
    QMutexLocker locker(&mutex);
    DBusError dbusError;
    DBusConnection *dbusConnection;

    if (ResourceEngine::libresourceConnection == NULL) {

        dbus_error_init(&dbusError);
        dbusConnection = dbus_bus_get_private(DBUS_BUS_SYSTEM, &dbusError);
        if (dbus_error_is_set(&dbusError)) {
            qDebug("Error getting the system bus: %s", dbusError.message);
            dbus_error_free(&dbusError);
            return false;
        }
        dbus_error_free(&dbusError);
        DBUSConnectionEventLoop::addConnection(dbusConnection);

        ResourceEngine::libresourceConnection = resproto_init(RESPROTO_ROLE_CLIENT, RESPROTO_TRANSPORT_DBUS,
                                              connectionIsUp, dbusConnection);
        if (ResourceEngine::libresourceConnection == NULL) {
            qDebug("resproto_init failed!");
            return false;
        }
        ResourceEngine::libresourceUsers = 1;
        resproto_set_handler(ResourceEngine::libresourceConnection, RESMSG_UNREGISTER, handleUnregisterMessage);
        resproto_set_handler(ResourceEngine::libresourceConnection, RESMSG_GRANT, handleGrantMessage);
        resproto_set_handler(ResourceEngine::libresourceConnection, RESMSG_ADVICE, handleAdviceMessage);
        resproto_set_handler(ResourceEngine::libresourceConnection, RESMSG_RELEASE, handleReleaseMessage);
        engineMap.insert(ResourceEngine::libresourceConnection, this);
    }
    else {
        ResourceEngine::libresourceUsers += 1;
        engineMap.insert(ResourceEngine::libresourceConnection, this);
    }

    qDebug("ResourceEngine (%u, %p) is now initialized. %d users",
           identifier, ResourceEngine::libresourceConnection,
           ResourceEngine::libresourceUsers);
    return true;
}

static void handleUnregisterMessage(resmsg_t *message, resset_t *libresourceSet, void *)
{
    qDebug("**************** %s() - locking....", __FUNCTION__);
    QMutexLocker locker(&mutex);
    if (NULL == libresourceSet->userdata) {
        qDebug("IGNORING unregister, no context");
        return;
    }
    ResourceEngine *engine = reinterpret_cast<ResourceEngine *>(libresourceSet->userdata);
    qDebug("recv: unregister: id=%d, engine->id() = %d", message->any.id, engine->id());
    if(engine->id() != message->any.id) {
        qDebug("Received an unregister notification, but it is not for us. Ignoring (%d != %d)",
               message->any.id, engine->id());
        return;
    }

    engine->disconnected();
}

void ResourceEngine::disconnected()
{
    qDebug("ResourceEngine(%d) - disconnected", identifier);
    connected = false;
    emit disconnectedFromManager();
}

static void handleGrantMessage(resmsg_t *message, resset_t *libresourceSet, void *)
{
    qDebug("**************** %s() - locking....", __FUNCTION__);
    QMutexLocker locker(&mutex);
    if (NULL == libresourceSet->userdata) {
        qDebug("IGNORING grant, no context: type=0x%04x, id=0x%04x, reqno=0x%04x, resc=0x%04x",
               message->notify.type, message->notify.id, message->notify.reqno, message->notify.resrc);
        return;
    }
    ResourceEngine *engine = reinterpret_cast<ResourceEngine *>(libresourceSet->userdata);
    qDebug("recv: grant: type=%d, id=%d, reqno=%d, resc=0x%04x engine->id() = %d",
           message->notify.type, message->notify.id, message->notify.reqno,
           message->notify.resrc, engine->id());
    if(engine->id() != message->any.id) {
        qDebug("Received a grant message, but it is not for us. Ignoring (%d != %d)",
               engine->id(), message->any.id);
        return;
    }
    engine->receivedGrant(&(message->notify));
}

void ResourceEngine::receivedGrant(resmsg_notify_t *notifyMessage)
{
    qDebug("ResourceEngine(%d) -- receivedGrant: type=0x%04x, id=0x%04x, reqno=0x%04x, resc=0x%04x",
           identifier, notifyMessage->type, notifyMessage->id, notifyMessage->reqno, notifyMessage->resrc);

    if (notifyMessage->resrc == 0) {

        bool unkownRequest                = !messageMap.contains(notifyMessage->reqno);
        resmsg_type_t originalMessageType =  messageMap.take(notifyMessage->reqno);

        qDebug("ResourceEngine(%d) -- originalMessageType=%u", identifier, originalMessageType);

        if (unkownRequest ) {
            //we don't know this req number => it must be a server override
            qDebug("ResourceEngine(%d) -- emiting signal resourcesLost()", identifier);
            emit resourcesLost(allResourcesToBitmask(resourceSet));

        }else if ( originalMessageType == RESMSG_UPDATE ) {
            //An app can loose all resources with update() or if it had no resources,
            //it can be ACKed saying that the update() was OK but you have no resources yet.

            if ( resourceSet->hasResourcesGranted() ) {
                qDebug("ResourceEngine(%d) -- emitting signal resourcesLost() for update", identifier);
                emit resourcesLost(allResourcesToBitmask(resourceSet));
            }else
            {
                if ( resourceSet->alwaysGetReply() ) {
                    //If alwaysReply is on and we didn't have resources at update() then we come from here to updateOK()
                    qDebug("ResourceEngine(%d) -- emitting signal updateOK() via receivedGrant.", identifier);
                    emit updateOK(true);
                }
                else {
                    emit updateOK(false);
                }
            }

        }else if (originalMessageType == RESMSG_ACQUIRE && resourceSet->alwaysGetReply() ) {
            qDebug("ResourceEngine(%d) -- request DENIED!", identifier);
            emit resourcesDenied();
        }
        else if (originalMessageType == RESMSG_RELEASE) {
            qDebug("ResourceEngine(%d) -- confirmation to release", identifier);
            emit resourcesReleased();
        }
        else {
            qDebug("ResourceEngine(%d) -- Ignoring the receivedGrant because original message unknown.", identifier);
        }
    }
    else {

        qDebug("ResourceEngine(%d) - emitting signal resourcesGranted(%02x).", identifier, notifyMessage->resrc);
        emit resourcesGranted(notifyMessage->resrc);
    }

    messageMap.remove(notifyMessage->reqno);
}


static void handleReleaseMessage(resmsg_t *message, resset_t *rs, void *)
{
    qDebug("**************** %s() - locking....", __FUNCTION__);
    QMutexLocker locker(&mutex);
    if (NULL == rs->userdata) {
        qDebug("IGNORING release, no context");
        return;
    }
    ResourceEngine *engine = reinterpret_cast<ResourceEngine *>(rs->userdata);
    qDebug("recv: release: type=%d, id=%d, reqno=%d, resc=0x%04x engine->id() = %d",
           message->notify.type, message->notify.id, message->notify.reqno,
           message->notify.resrc, engine->id());
    if(engine->id() != message->any.id) {
        qDebug("Received an advice message, but it is not for us. Ignoring (%d != %d)",
               engine->id(), message->any.id);
        return;
    }

    engine->receivedRelease(&(message->notify));
}

void ResourceEngine::receivedRelease(resmsg_notify_t *message)
{
    uint32_t allResources = allResourcesToBitmask(resourceSet);
    qDebug("ResourceEngine(%d) - %s: have: %02x got %02x", identifier, __FUNCTION__, allResources, message->resrc);
    emit resourcesReleasedByManager();
}

static void handleAdviceMessage(resmsg_t *message, resset_t *libresourceSet, void *)
{
    qDebug("**************** %s() - locking....", __FUNCTION__);
    QMutexLocker locker(&mutex);
    if (NULL == libresourceSet->userdata) {
        qDebug("IGNORING advice, no context");
        return;
    }
    ResourceEngine *engine = reinterpret_cast<ResourceEngine *>(libresourceSet->userdata);
    qDebug("recv: advice: type=%d, id=%d, reqno=%d, resc=0x%04x engine->id() = %d",
           message->notify.type, message->notify.id, message->notify.reqno,
           message->notify.resrc, engine->id());
    if(engine->id() != message->any.id) {
        qDebug("Received an advice message, but it is not for us. Ignoring (%d != %d)",
               engine->id(), message->any.id);
        return;
    }

    engine->receivedAdvice(&(message->notify));
}

void ResourceEngine::receivedAdvice(resmsg_notify_t *message)
{
    uint32_t allResources = allResourcesToBitmask(resourceSet);
    qDebug("ResourceEngine(%d) - %s: have: %02x got %02x", identifier, __FUNCTION__, allResources, message->resrc);
    emit resourcesBecameAvailable(message->resrc);
}

bool ResourceEngine::connectToManager()
{
    qDebug("ResourceEngine(%d)::%s() - **************** locking....", identifier, __FUNCTION__);
    QMutexLocker locker(&mutex);
    if (isConnecting) {
        qDebug("ResourceEngine::%s().... allready connecting, ignoring request", __FUNCTION__);
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

    qDebug("ResourceEngine(%d) - ResourceEngine is now connecting(%d, %d, %d)",
           identifier, resourceMessage.record.id, resourceMessage.record.reqno,
           resourceMessage.record.rset.all);
    libresourceSet = resconn_connect(ResourceEngine::libresourceConnection, &resourceMessage,
                                     statusCallbackHandler);                                     
    if (libresourceSet == NULL)
        return false;
    libresourceSet->userdata = this; //save our context
    //locker.unlock();
    qDebug("ResourceEngine(%d)::%s() - **************** unlocked! returning true", identifier, __FUNCTION__);
    return true;
}

bool ResourceEngine::disconnectFromManager()
{
    qDebug("ResourceEngine(%d)::%s() - **************** locking....", identifier, __FUNCTION__);
    QMutexLocker locker(&mutex);
    resmsg_t resourceMessage;
    memset(&resourceMessage, 0, sizeof(resmsg_t));

    qDebug("ResourceEngine(%d)::%s() - disconnecting from manager - %p",
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
        qDebug("Converted Resource 0x%02x to 0x%02x", resourceList[i]->type(), bits);
        bitmask += bits;
    }
    qDebug("All resources as bitmask is 0x%04x", bitmask);
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
    default:
        qDebug("Unknown resource Type %d", quint32(type));
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
    qDebug("**************** %s().... %d", __FUNCTION__, __LINE__);
    //qDebug("**************** %s() - locking....", __FUNCTION__);
    QMutexLocker locker(&mutex);
    if (NULL == libresourceSet->userdata) {
        qDebug("IGNORING status message, no context: type=0x%04x, id=0x%04x, reqno=0x%04x, errcod=%d",
               message->status.type, message->status.id, message->status.reqno, message->status.errcod);
        return;
    }
    qDebug("**************** %s().... %d", __FUNCTION__, __LINE__);
    ResourceEngine *resourceEngine = reinterpret_cast<ResourceEngine *>(libresourceSet->userdata);
    qDebug("**************** %s().... %d", __FUNCTION__, __LINE__);
    qDebug("**************** %s().... %d", __FUNCTION__, __LINE__);
    qDebug("recv: status: id=%d, engine->id() = %d", message->any.id, resourceEngine->id());
    qDebug("**************** %s().... %d", __FUNCTION__, __LINE__);
    if(resourceEngine->id() != libresourceSet->id) {
        qDebug("Received a status notification, but it is not for us. Ignoring (%d != %d)",
               resourceEngine->id(), libresourceSet->id);
        return;
    }
    qDebug("Received a status notification");
    if (message->type != RESMSG_STATUS) {
        qDebug("Invalid message type.. (got %x, expected %x", message->type, RESMSG_STATUS);
        return;
    }
    if (message->status.errcod) {
        resourceEngine->handleError(message->status.reqno, message->status.errcod, message->status.errmsg);
    }
    else {
        qDebug("Received a status message with id %02x and #:%u", message->status.id, message->status.reqno);
        if(!resourceEngine->isConnectedToManager() && resourceEngine->toBeDeleted()) {
            qDebug("%s(%d) - delete resourceEngine %p", __FUNCTION__, __LINE__, resourceEngine);
            delete resourceEngine;
        }
        else {
            resourceEngine->handleStatusMessage(message->status.reqno);
        }
    }
}

void ResourceEngine::handleStatusMessage(quint32 requestNo)
{
    resmsg_type_t originalMessageType = messageMap.value(requestNo);
    qDebug("Received a status message: %u(0x%02x)", requestNo, originalMessageType);
    if (originalMessageType == RESMSG_REGISTER) {
        qDebug("ResourceEngine(%d) - connected!", identifier);
        connected = true;
        isConnecting = false;
        emit connectedToManager();
        messageMap.remove(requestNo);
    }
    else if (originalMessageType == RESMSG_UNREGISTER) {
        qDebug("ResourceEngine(%d) - disconnected!", identifier);
        connected = false;
        emit disconnectedFromManager();
        messageMap.remove(requestNo);
    }
    else if(originalMessageType == RESMSG_UPDATE) {
        qDebug("ResourceEngine(%d) - Update status", identifier);
        //We only come here if status ok.

        //bool hadGrantsWhenSentUpdate = false;

        //if ( wasInAcquireMode.contains(requestNo) )
        //   hadGrantsWhenSentUpdate = wasInAcquireMode.take(requestNo);

        //if ( !hadGrantsWhenSentUpdate  &&  !resourceSet->alwaysGetReply() ) {

            //If alwaysReply is off and we didn't have resources at update() emit from here to
            //updateOK() (i.e. ACK that the set we are interested in is changed). Or if alwayReply
            // is off and our update does not change the granted set.
            qDebug("ResourceEngine(%d) -- handleStatusMessage.", identifier);
            emit updateOK(false);
        //}

    }
    else if(originalMessageType == RESMSG_ACQUIRE) {
        qDebug("ResourceEngine(%d) - Acquire status", identifier);
    }
    else if(originalMessageType == RESMSG_RELEASE) {
        qDebug("ResourceEngine(%d) - Release status", identifier);
    }
    else {
        messageMap.remove(requestNo);
    }
}

void ResourceEngine::handleError(quint32 requestNo, qint32 code, const char *message)
{
    resmsg_type_t originalMessageType = messageMap.take(requestNo);
    qDebug("ResourceEngine(%d) - Error on request %u(0x%02x): %d - %s",
           identifier, requestNo, originalMessageType, code, message);
    messageMap.remove(requestNo);

    qDebug("emitting errorCallback");
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
    qDebug("ResourceEngine(%d)::%s() - **************** locking....", identifier, __FUNCTION__);
    QMutexLocker locker(&mutex);
    resmsg_t message;
    memset(&message, 0, sizeof(resmsg_t));

    message.possess.type = RESMSG_ACQUIRE;
    message.possess.id    = resourceSet->id();
    message.possess.reqno = ++requestId;

    messageMap.insert(requestId, RESMSG_ACQUIRE);

    qDebug("ResourceEngine(%d) - acquire %u:%u", identifier, resourceSet->id(), requestId);
    int success = resproto_send_message(libresourceSet, &message, statusCallbackHandler);

    if(!success)
        return false;
    else
        return true;
}

bool ResourceEngine::releaseResources()
{
    qDebug("ResourceEngine(%d)::%s() - **************** locking....", identifier, __FUNCTION__);
    QMutexLocker locker(&mutex);
    resmsg_t message;
    memset(&message, 0, sizeof(resmsg_t));

    message.possess.type = RESMSG_RELEASE;
    message.possess.id    = resourceSet->id();
    message.possess.reqno = ++requestId;

    messageMap.insert(requestId, RESMSG_RELEASE);
    qDebug("ResourceEngine(%d) - release %u:%u", identifier, resourceSet->id(), requestId);
    int success = resproto_send_message(libresourceSet, &message, statusCallbackHandler);

    if(!success)
        return false;
    else
        return true;
}

bool ResourceEngine::updateResources()
{
    qDebug("ResourceEngine(%d)::%s() - **************** locking....", identifier, __FUNCTION__);
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

    qDebug("ResourceEngine(%d) - update %u:%u", identifier, resourceSet->id(), requestId);
    int success = resproto_send_message(libresourceSet, &message, statusCallbackHandler);

    if(!success)
        return false;
    else
        return true;
}

bool ResourceEngine::registerAudioProperties(const QString &audioGroup, quint32 pid,
                                              const QString &name, const QString &value)
{
    qDebug("ResourceEngine(%d)::%s() - **************** locking....", identifier, __FUNCTION__);
    QMutexLocker locker(&mutex);
    resmsg_t message;
    memset(&message, 0, sizeof(resmsg_t));
    QByteArray groupBa, nameBa, valueBa;

    if (pid != 0) {
        message.audio.pid = pid;
        qDebug("ResourceEngine(%d) - audio pid %u", identifier, pid);
    }
    if (!audioGroup.isEmpty() && !audioGroup.isNull()) {
        resmsg_audio_t    *audio;
        groupBa = audioGroup.toLatin1();
        message.audio.group = groupBa.data();
        qDebug("ResourceEngine(%d) - audio group: %s", identifier, message.audio.group);
        audio    = &message.audio;
    }
    if (!name.isEmpty() && !name.isNull() && !value.isEmpty() && !value.isNull()) {
        nameBa = name.toLatin1();
        valueBa = value.toLatin1();
        message.audio.property.name = nameBa.data();
        message.audio.property.match.method  = resmsg_method_equals;
        message.audio.property.match.pattern = valueBa.data();
        qDebug("ResourceEngine(%d) - audio stream tag is %s:%s",
               identifier, message.audio.property.name, message.audio.property.match.pattern);
    }

    message.audio.type = RESMSG_AUDIO;
    message.audio.id    = resourceSet->id();
    message.audio.reqno = ++requestId;

    message.audio.type  = RESMSG_AUDIO;

    messageMap.insert(requestId, RESMSG_AUDIO);

    qDebug("ResourceEngine(%d) - audio %u:%u", identifier, resourceSet->id(), requestId);
    int success = resproto_send_message(libresourceSet, &message, statusCallbackHandler);
    qDebug("ResourceEngine(%d) - resproto_send_message returned %d", identifier, success);

    if(!success)
        return false;
    else
        return true;
}

bool ResourceEngine::registerVideoProperties(quint32 pid)
{
    qDebug("ResourceEngine(%d)::%s() - **************** locking....", identifier, __FUNCTION__);
    QMutexLocker locker(&mutex);
    resmsg_t message;
    memset(&message, 0, sizeof(resmsg_t));

    if (pid <= 0) {
        qDebug("ResourceEngine(%d) - erroneous pid %u", identifier, pid);
        return false;
    }

    message.video.pid   = pid;
    message.video.type  = RESMSG_VIDEO;
    message.video.id    = resourceSet->id();
    message.video.reqno = ++requestId;
    message.video.type  = RESMSG_VIDEO;

    messageMap.insert(requestId, RESMSG_VIDEO);

    qDebug("ResourceEngine(%d) - video %u:%u", identifier, resourceSet->id(), requestId);
    int success = resproto_send_message(libresourceSet, &message, statusCallbackHandler);
    qDebug("ResourceEngine(%d) - resproto_send_message returned %d", identifier, success);

    if(!success)
        return false;
    else
        return true;
}

static void connectionIsUp(resconn_t *connection)
{
    qDebug("**************** %s() - locking....", __FUNCTION__);
    QMutexLocker locker(&mutex);

    qDebug("connection is up");

    QList<ResourceEngine*> engines = engineMap.values(connection);
    for (int i = 0; i < engines.size(); ++i) {
        ResourceEngine *resourceEngine = engines.at(i);
        resourceEngine->handleConnectionIsUp(connection);
    }
}

void ResourceEngine::handleConnectionIsUp(resconn_t *connection)
{

    if(ResourceEngine::libresourceConnection == connection) {
        qDebug("ResourceEngine(%d) - connected to manager, connection=%p", identifier, connection);
        emit connectedToManager();
    }
    else {
        qDebug("ResourceEngine(%d) - ignoring Connection is up, it is not for us (%p != %p)",
               identifier, ResourceEngine::libresourceConnection, connection);
    }
}

quint32 ResourceEngine::id()
{
    return identifier;
}

