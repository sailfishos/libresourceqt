#include "resource-engine.h"
#include <dbus/dbus.h>

using namespace ResourcePolicy;

static inline quint32 allResourcesToBitmask(const ResourceSet *resourceSet);
static inline quint32 optionalResourcesToBitmask(const ResourceSet *resourceSet);

static void connectionIsUp(resconn_t *connection);
static void statusCallbackHandler(resset_t *rset, resmsg_t *msg);
static void handleUnregisterMessage(resmsg_t *, resset_t *, void *data);
static void handleGrantMessage(resmsg_t *msg, resset_t *rs, void *data);
static void handleAdviceMessage(resmsg_t *msg, resset_t *rs, void *data);

ResourceEngine::ResourceEngine(ResourceSet *resourceSet)
        : QObject(resourceSet), connected(false), resourceSet(resourceSet),
        dbusEngine(NULL), libresourceConnection(NULL), libresourceSet(NULL),
        requestId(0), messageMap(), connectionMode(0)
{
    if (resourceSet->alwaysGetReply()) {
        connectionMode += RESMSG_MODE_ALWAYS_REPLY;
    }
    if (resourceSet->willAutoRelease()) {
        connectionMode += RESOURCE_AUTO_RELEASE;
    }
    qDebug("connectionMode = %04x", connectionMode);
}

ResourceEngine::~ResourceEngine()
{
    delete dbusEngine;
    if (libresourceSet != NULL)
        libresourceSet->userdata = NULL;
    //need to destroy all libresource structures, but how?
}

bool ResourceEngine::initialize()
{
    DBusError dbusError;
    DBusConnection *dbusConnection;
    dbusEngine = new DBUSConnectionEventLoop;
    if(dbusEngine == NULL)
        return false;

    dbus_error_init(&dbusError);
    dbusConnection = dbus_bus_get(DBUS_BUS_SESSION, &dbusError);
    if (dbus_error_is_set(&dbusError)) {
        qDebug("Error getting the session bus: %s", dbusError.message);
        dbus_error_free(&dbusError);
        return false;
    }
    dbus_error_free(&dbusError);
    dbusEngine->addConnection(dbusConnection);
    libresourceConnection = resproto_init(RESPROTO_ROLE_CLIENT, RESPROTO_TRANSPORT_DBUS,
                                          connectionIsUp, dbusConnection);
    if (libresourceConnection == NULL) {
        return NULL;
    }

    resproto_set_handler(libresourceConnection, RESMSG_UNREGISTER, handleUnregisterMessage);
    resproto_set_handler(libresourceConnection, RESMSG_GRANT, handleGrantMessage);
    resproto_set_handler(libresourceConnection, RESMSG_ADVICE, handleAdviceMessage);
    qDebug("ResourceEngine (%p) is now initialized.", this);
    return true;
}

static void handleUnregisterMessage(resmsg_t *, resset_t *libresourceSet, void *)
{
    if (NULL == libresourceSet->userdata) {
        qDebug("IGNORING unregister, no context");
        return;
    }
    ResourceEngine *engine = reinterpret_cast<ResourceEngine *>(libresourceSet->userdata);

    engine->disconnected();
}

void ResourceEngine::disconnected()
{
    qDebug("disconnected");
    connected = false;
    emit disconnectedFromManager();
}

static void handleGrantMessage(resmsg_t *message, resset_t *libresourceSet, void *)
{
    if (NULL == libresourceSet->userdata) {
        qDebug("IGNORING grant, no context: type=0x%04x, id=0x%04x, reqno=0x%04x, resc=0x%04x",
               message->notify.type, message->notify.id, message->notify.reqno, message->notify.resrc);
        return;
    }
    ResourceEngine *engine = reinterpret_cast<ResourceEngine *>(libresourceSet->userdata);
    engine->receivedGrant(&(message->notify));
}

void ResourceEngine::receivedGrant(resmsg_notify_t *notifyMessage)
{
    qDebug("receivedGrant: type=0x%04x, id=0x%04x, reqno=0x%04x, resc=0x%04x",
           notifyMessage->type, notifyMessage->id, notifyMessage->reqno, notifyMessage->resrc);
    if (notifyMessage->resrc == 0) {
        bool unkownRequest = !messageMap.contains(notifyMessage->reqno);
        resmsg_type_t originaloriginalMessageType = messageMap.take(notifyMessage->reqno);
        qDebug("lost resources, originaloriginalMessageType=%u", originaloriginalMessageType);
        if (unkownRequest) {
            //we don't know this req number => it must be a server override.
            qDebug("emiting signal resourcesLost()");
            emit resourcesLost(allResourcesToBitmask(resourceSet));
        }
        else if (originaloriginalMessageType == RESMSG_ACQUIRE) {
                qDebug("request DENIED!");
                emit resourcesDenied();
        }
        else if (originaloriginalMessageType == RESMSG_RELEASE) {
                qDebug("confirmnation to release");
                emit resourcesReleased();
        }
        else {
            qDebug("Ignoring the receivedGrant");
        }
    }
    else {
        qDebug("emiting signal resourcesGranted(%02x)", notifyMessage->resrc);
        emit resourcesGranted(notifyMessage->resrc);
    }
}

static void handleAdviceMessage(resmsg_t *message, resset_t *libresourceSet, void *)
{
    qDebug("ADVICE: type=0x%04x, id=0x%04x, reqno=0x%04x, resc=0x%04x",
           message->notify.type, message->notify.id, message->notify.reqno, message->notify.resrc);

    if (NULL == libresourceSet->userdata) {
        qDebug("IGNORING advice, no context");
        return;
    }
    ResourceEngine *engine = reinterpret_cast<ResourceEngine *>(libresourceSet->userdata);

    engine->receivedAdvice(&(message->notify));
}

void ResourceEngine::receivedAdvice(resmsg_notify_t *message)
{
    qDebug("%s: %04x", __FUNCTION__, message->resrc);
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

    qDebug("ResourceEngine is now connecting...");
    libresourceSet = resconn_connect(libresourceConnection, &resourceMessage,
                                     statusCallbackHandler);

    libresourceSet->userdata = this; //save our context
    return true;
}

bool ResourceEngine::disconnectFromManager()
{
    resmsg_t resourceMessage;
    memset(&resourceMessage, 0, sizeof(resmsg_t));

    qDebug("disconnecting from manager");

    resourceMessage.record.type = RESMSG_UNREGISTER;
    resourceMessage.record.id = resourceSet->id();
    resourceMessage.record.reqno = ++requestId;

    messageMap.insert(requestId, RESMSG_UNREGISTER);

    int r = resconn_disconnect(libresourceSet, &resourceMessage,
                               statusCallbackHandler);
    connected = false;
    if (r)
        return true;
    else
        return false;
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
    if (NULL == libresourceSet->userdata) {
        qDebug("IGNORING status message, no context: type=0x%04x, id=0x%04x, reqno=0x%04x, errcod=%d",
               message->status.type, message->status.id, message->status.reqno, message->status.errcod);
        return;
    }
    ResourceEngine *resourceEngine = reinterpret_cast<ResourceEngine *>(libresourceSet->userdata);
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
        resourceEngine->handleStatusMessage(message->status.reqno);
    }
}

void ResourceEngine::handleStatusMessage(quint32 requestNo)
{
    resmsg_type_t originalMessageType = messageMap.value(requestNo);
    qDebug("Received a status message: %u(0x%02x)", requestNo, originalMessageType);
    if (originalMessageType == RESMSG_REGISTER) {
        qDebug("connected!");
        connected = true;
        emit connectedToManager();
        messageMap.remove(requestNo);
    }
    else if (originalMessageType == RESMSG_UNREGISTER) {
        qDebug("disconnected!");
        connected = false;
        emit disconnectedFromManager();
        messageMap.remove(requestNo);
    }
    else if(originalMessageType == RESMSG_UPDATE) {
        qDebug("Update status");
    }
    else if(originalMessageType == RESMSG_ACQUIRE) {
        qDebug("Acquire status");
    }
    else if(originalMessageType == RESMSG_RELEASE) {
        qDebug("Release status");
    }
}

void ResourceEngine::handleError(quint32 requestNo, qint32 code, const char *message)
{
    resmsg_type_t originalMessageType = messageMap.take(requestNo);
    qDebug("Error on request %u(0x%02x): %d - %s", requestNo, originalMessageType, code, message);
}

bool ResourceEngine::isConnectedToManager()
{
    return connected;
}

bool ResourceEngine::acquireResources()
{
    resmsg_t message;
    memset(&message, 0, sizeof(resmsg_t));

    message.possess.type = RESMSG_ACQUIRE;
    message.possess.id    = resourceSet->id();
    message.possess.reqno = ++requestId;

    messageMap.insert(requestId, RESMSG_ACQUIRE);

    qDebug("acquire %u:%u", resourceSet->id(), requestId);
    int success = resproto_send_message(libresourceSet, &message, statusCallbackHandler);

    if(!success)
        return false;
    else
        return true;
}

bool ResourceEngine::releaseResources()
{
    resmsg_t message;
    memset(&message, 0, sizeof(resmsg_t));

    message.possess.type = RESMSG_RELEASE;
    message.possess.id    = resourceSet->id();
    message.possess.reqno = ++requestId;

    messageMap.insert(requestId, RESMSG_RELEASE);
    qDebug("release %u:%u", resourceSet->id(), requestId);
    int success = resproto_send_message(libresourceSet, &message, statusCallbackHandler);

    if(!success)
        return false;
    else
        return true;
}

bool ResourceEngine::updateResources()
{
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

    qDebug("update %u:%u", resourceSet->id(), requestId);
    int success = resproto_send_message(libresourceSet, &message, statusCallbackHandler);

    if(!success)
        return false;
    else
        return true;
}

bool ResourceEngine::registerAudioProperties(const QString &audioGroup, quint32 pid,
                                              const QString &name, const QString &value)
{
    resmsg_t message;
    memset(&message, 0, sizeof(resmsg_t));
    QByteArray groupBa, nameBa, valueBa;

    if (pid != 0) {
        message.audio.pid = pid;
        qDebug("audio pid %u", pid);
    }
    if (!audioGroup.isEmpty() && !audioGroup.isNull()) {
        resmsg_audio_t    *audio;
        groupBa = audioGroup.toLatin1();
        message.audio.group = groupBa.data();
        qDebug("audio group: %s", message.audio.group);
        audio    = &message.audio;
    }
    if (!name.isEmpty() && !name.isNull() && !value.isEmpty() && !value.isNull()) {
        nameBa = name.toLatin1();
        valueBa = value.toLatin1();
        message.audio.property.name = nameBa.data();
        message.audio.property.match.method  = resmsg_method_equals;
        message.audio.property.match.pattern = valueBa.data();
        qDebug("audio stream tag is %s:%s", message.audio.property.name, message.audio.property.match.pattern);
    }

    message.audio.type = RESMSG_AUDIO;
    message.audio.id    = resourceSet->id();
    message.audio.reqno = ++requestId;

    message.audio.type  = RESMSG_AUDIO;

    messageMap.insert(requestId, RESMSG_AUDIO);

    qDebug("audio %u:%u", resourceSet->id(), requestId);
    int success = resproto_send_message(libresourceSet, &message, statusCallbackHandler);

    if(!success)
        return false;
    else
        return true;
}

static void connectionIsUp(resconn_t *connection)
{
    if (NULL == connection->dbus.rsets->userdata) {
        qDebug("IGNORING connectionIsUp");
        return;
    }
    ResourceEngine *resourceEngine;

    resourceEngine = reinterpret_cast<ResourceEngine *>(connection->dbus.rsets->userdata);

    resourceEngine->handleConnectionIsUp();
}

void ResourceEngine::handleConnectionIsUp()
{
    emit connectedToManager();
}

