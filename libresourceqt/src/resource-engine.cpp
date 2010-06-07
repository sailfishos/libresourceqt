#include "resource-engine.h"
#include <dbus/dbus.h>

using namespace ResourcePolicy;

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

ResourceEngine::ResourceEngine(ResourceSet *resourceSet)
        : QObject(), connected(false), resourceSet(resourceSet),
        libresourceSet(NULL), requestId(0), messageMap(), connectionMode(0),
        identifier(resourceSet->id()), aboutToBeDeleted(false), isConnecting(false)
{
    if (resourceSet->alwaysGetReply()) {
        connectionMode += RESMSG_MODE_ALWAYS_REPLY;
    }
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
        dbusConnection = dbus_bus_get(DBUS_BUS_SYSTEM, &dbusError);
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
    }
    else {
        ResourceEngine::libresourceUsers += 1;
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
    qDebug("ResourceEngine(%d) - receivedGrant: type=0x%04x, id=0x%04x, reqno=0x%04x, resc=0x%04x",
           identifier, notifyMessage->type, notifyMessage->id, notifyMessage->reqno, notifyMessage->resrc);
    if (notifyMessage->resrc == 0) {
        bool unkownRequest = !messageMap.contains(notifyMessage->reqno);
        resmsg_type_t originaloriginalMessageType = messageMap.take(notifyMessage->reqno);
        qDebug("ResourceEngine(%d) - lost resources, originaloriginalMessageType=%u", 
               identifier, originaloriginalMessageType);
        if (unkownRequest) {
            //we don't know this req number => it must be a server override.
            qDebug("ResourceEngine(%d) - emiting signal resourcesLost()", identifier);
            emit resourcesLost(allResourcesToBitmask(resourceSet));
        }
        else if (originaloriginalMessageType == RESMSG_ACQUIRE) {
                qDebug("ResourceEngine(%d) - request DENIED!", identifier);
                emit resourcesDenied();
        }
        else if (originaloriginalMessageType == RESMSG_RELEASE) {
                qDebug("ResourceEngine(%d) - confirmation to release", identifier);
                emit resourcesReleased();
        }
        else {
            qDebug("ResourceEngine(%d) - Ignoring the receivedGrant", identifier);
        }
    }
    else {
        qDebug("ResourceEngine(%d) - emiting signal resourcesGranted(%02x)",
               identifier, notifyMessage->resrc);
        emit resourcesGranted(notifyMessage->resrc);
    }
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
    qDebug("ResourceEngine(%d) - %s: %04x", identifier, __FUNCTION__, message->resrc);
    uint32_t allResources = allResourcesToBitmask(resourceSet);
    if(message->resrc < allResources) {
        emit resourcesLost(allResources-message->resrc);
    }
    else {
        emit resourcesBecameAvailable(message->resrc);
    }
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
    resourceMessage.record.rset.mask = connectionMode;

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

    int r = resconn_disconnect(libresourceSet, &resourceMessage,
                               statusCallbackHandler);
    if (r)
        return true;
    else
        return false;
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
        bitmask += resourceTypeToLibresourceType(resourceList[i]->type());
    }
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
    default:
        return 0;
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
    }
    else if(originalMessageType == RESMSG_ACQUIRE) {
        qDebug("ResourceEngine(%d) - Acquire status", identifier);
    }
    else if(originalMessageType == RESMSG_RELEASE) {
        qDebug("ResourceEngine(%d) - Release status", identifier);
    }
}

void ResourceEngine::handleError(quint32 requestNo, qint32 code, const char *message)
{
    resmsg_type_t originalMessageType = messageMap.take(requestNo);
    qDebug("ResourceEngine(%d) - Error on request %u(0x%02x): %d - %s",
           identifier, requestNo, originalMessageType, code, message);
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
    message.record.rset.mask = connectionMode;

    QByteArray ba = resourceSet->applicationClass().toLatin1();
    message.record.klass = ba.data();

    messageMap.insert(requestId, RESMSG_UPDATE);

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

    if(!success)
        return false;
    else
        return true;
}

static void connectionIsUp(resconn_t *connection)
{
    qDebug("**************** %s() - locking....", __FUNCTION__);
    QMutexLocker locker(&mutex);
    if (NULL == connection->dbus.rsets->userdata) {
        qDebug("IGNORING connectionIsUp");
        return;
    }
    ResourceEngine *resourceEngine;

    resourceEngine = reinterpret_cast<ResourceEngine *>(connection->dbus.rsets->userdata);
    qDebug("connection is up");
    resourceEngine->handleConnectionIsUp(connection);
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

