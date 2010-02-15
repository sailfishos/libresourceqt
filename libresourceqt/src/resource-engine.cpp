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
        requestId(0), messageMap(), mode(0)
{
}

ResourceEngine::~ResourceEngine()
{
}

bool ResourceEngine::initialize()
{
    DBusError dbusError;
    DBusConnection *dbusConnection;
    dbusEngine = new DBUSConnectionEventLoop;
    if(dbusEngine == NULL)
        return false;

    dbus_error_init(&dbusError);
    dbusConnection = dbus_bus_get(DBUS_BUS_SYSTEM, &dbusError);
    if (dbus_error_is_set(&dbusError)) {
        qDebug("Error getting the system bus: %s", dbusError.message);
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

    return true;
}

static void handleUnregisterMessage(resmsg_t *, resset_t *, void *data)
{
    ResourceEngine *engine = reinterpret_cast<ResourceEngine *>(data);

    engine->disconnected();
}

void ResourceEngine::disconnected()
{
    qDebug("disconnected");
    connected = false;
    emit disconnectedFromManager();
}

static void handleGrantMessage(resmsg_t *msg, resset_t *, void *data)
{
    ResourceEngine *engine = reinterpret_cast<ResourceEngine *>(data);

    engine->receivedGrant(&(msg->notify));
}

void ResourceEngine::receivedGrant(resmsg_notify_t *notifyMessage)
{
    qDebug("received a grant message");
    if(notifyMessage->resrc == 0) {
        emit resourcesDenied();
    }
    else {
        emit resourcesAcquired(notifyMessage->resrc);
    }
}

static void handleAdviceMessage(resmsg_t *msg, resset_t *, void *data)
{
    ResourceEngine *engine = reinterpret_cast<ResourceEngine *>(data);

    engine->receivedAdvice(&(msg->notify));
}

void ResourceEngine::receivedAdvice(resmsg_notify_t *notifyMessage)
{
    char buf[80];
    resmsg_res_str(notifyMessage->resrc, buf, sizeof(buf));
    qDebug("%s: %s", __FUNCTION__, buf);
}

bool ResourceEngine::connect()
{
    resmsg_t resourceMessage;
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

    resourceMessage.record.mode = mode;

    libresourceSet = resconn_connect(libresourceConnection, &resourceMessage,
                                     statusCallbackHandler);

    libresourceSet->userdata = this; //save our context
    return true;
}

bool ResourceEngine::disconnect()
{
    resmsg_t resourceMessage;
    resourceMessage.record.type = RESMSG_UNREGISTER;
    resourceMessage.record.id = resourceSet->id();
    resourceMessage.record.reqno = ++requestId;

    messageMap.insert(requestId, RESMSG_UNREGISTER);

    uint32_t allResources, optionalResources;
    allResources = allResourcesToBitmask(resourceSet);
    optionalResources = optionalResourcesToBitmask(resourceSet);

    resourceMessage.record.rset.all = allResources;
    resourceMessage.record.rset.opt = optionalResources;
    resourceMessage.record.rset.share = 0;
    resourceMessage.record.rset.mask = mode;

    QByteArray ba = resourceSet->applicationClass().toLatin1();
    resourceMessage.record.klass = ba.data();

    resourceMessage.record.mode = 0; //No auto release

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
//        resourceEngine->handleStatusMessage(message->status.reqno);
        qDebug("Received a status message of type 0x%02x and #:%u", message->type, message->status.reqno);
    }
}

void ResourceEngine::handleStatusMessage(quint32 requestNo)
{
    resmsg_type_t messageType = messageMap.take(requestNo);
    qDebug("Received a status message: %u(0x%02x)", requestNo, messageType);
    if (messageType == RESMSG_REGISTER) {
        qDebug("connected!");
        connected = true;
        emit connectedToManager();
    }
    else if (messageType == RESMSG_UNREGISTER) {
        qDebug("disconnected!");
        connected = false;
        emit disconnectedFromManager();
    }
}

void ResourceEngine::handleError(quint32 requestNo, quint32 code, const char *message)
{
    resmsg_type_t messageType = messageMap.take(requestNo);
    qDebug("Error on request %u(0x%02x): %u - %s", requestNo, messageType, code, message);
}

bool ResourceEngine::isConnected()
{
    return connected;
}

bool ResourceEngine::acquireResources()
{
    resmsg_t message;
    memset(&message, 0, sizeof(resmsg_t));

    message.possess.type = RESMSG_ACQUIRE;
    message.any.id    = resourceSet->id();
    message.any.reqno = ++requestId;

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
    return false;
}

bool ResourceEngine::updateResources()
{
    return false;
}

bool ResourceEngine::registerAudioProperties(quint32 pid, QString streamName)
{
    return false;
}

static void connectionIsUp(resconn_t *connection)
{
    ResourceEngine *resourceEngine;

    resourceEngine = reinterpret_cast<ResourceEngine *>(connection->dbus.rsets->userdata);

    resourceEngine->handleConnectionIsUp();
}

void ResourceEngine::handleConnectionIsUp()
{
    emit connectedToManager();
}

void ResourceEngine::setMode(quint32 newMode)
{
    mode = newMode;
}

