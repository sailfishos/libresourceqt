#include "resource-engine.h"
#include <dbus/dbus.h>

using namespace ResourcePolicy;

static inline quint32 allResourcesToBitmask(const ResourceSet *resourceSet);
static inline quint32 optionalResourcesToBitmask(const ResourceSet *resourceSet);
static inline quint32 resourceTypeToLibresourceType(ResourceType type);

static void connectionIsUp(resconn_t *connection);
static void statusCallbackHandler(resset_t *rset, resmsg_t *msg);
static void handleUnregisterMessage(resmsg_t *, resset_t *, void *data);
static void handleGrantMessage(resmsg_t *msg, resset_t *rs, void *data);
static void handleAdviceMessage(resmsg_t *msg, resset_t *rs, void *data);

ResourceEngine::ResourceEngine(ResourceSet *resourceSet)
        : QObject(resourceSet), connected(false), resourceSet(resourceSet),
        libresourceConnection(NULL), libresourceSet(NULL), requestId(0),
        messageMap(), mode(0)
{
}

ResourceEngine::~ResourceEngine()
{
}

bool ResourceEngine::initialize()
{
    DBusError dbusError;
    DBusConnection *dbusConnection;

    dbus_error_init(&dbusError);
    dbusConnection = dbus_bus_get(DBUS_BUS_SYSTEM, &dbusError);
    if (dbus_error_is_set(&dbusError)) {
        qDebug("Error getting the system bus: %s", dbusError.message);
        dbus_error_free(&dbusError);
        return false;
    }
    dbus_error_free(&dbusError);
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
    char buf[80];
    resmsg_res_str(notifyMessage->resrc, buf, sizeof(buf));
    qDebug("%s: %s", __FUNCTION__, buf);
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

static inline quint32 resourceTypeToLibresourceType(ResourceType type)
{
    quint32 bitmask = 0;
    switch (type) {
    case AudioPlaybackType:
        bitmask += RESMSG_AUDIO_PLAYBACK;
        break;
    case VideoPlaybackType:
        bitmask += RESMSG_VIDEO_PLAYBACK;
        break;
    case AudioRecorderType:
        bitmask += RESMSG_AUDIO_RECORDING;
        break;
    case VideoRecorderType:
        bitmask += RESMSG_VIDEO_RECORDING;
        break;
    case VibraType:
        bitmask += RESMSG_VIBRA;
        break;
    case LedsType:
        bitmask += RESMSG_LEDS;
        break;
    case BacklightType:
        bitmask += RESMSG_BACKLIGHT;
        break;
    case SystemButtonType:
        bitmask += RESMSG_SYSTEM_BUTTON;
        break;
    case LockButtonType:
        bitmask += RESMSG_LOCK_BUTTON;
        break;
    case ScaleButtonType:
        bitmask += RESMSG_SCALE_BUTTON;
        break;
    case SnapButtonType:
        bitmask += RESMSG_SNAP_BUTTON;
        break;
    case LensCoverType:
        bitmask += RESMSG_LENS_COVER;
        break;
    default:
        break;
    }
    return bitmask;
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
    resourceEngine->handleStatusMessage(message->status.reqno);
}

void ResourceEngine::handleStatusMessage(quint32 requestNo)
{
    resmsg_type_t messageType = messageMap.take(requestNo);
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

bool ResourceEngine::isConnected()
{
    return connected;
}

bool ResourceEngine::acquireResources()
{
    return false;
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

