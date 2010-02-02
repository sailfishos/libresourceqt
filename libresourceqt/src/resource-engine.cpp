#include "resource-engine.h"
#include <dbus/dbus.h>
#include <Resource>

using namespace ResourcePolicy;

static void connectionIsUp(resconn_t *connection);
static inline quint32 allResourcesToBitmask(const ResourceSet *resourceSet);
static inline quint32 optionalResourcesToBitmask(const ResourceSet *resourceSet);
static inline quint32 sharedResourcesToBitmask(const ResourceSet *resourceSet);

ResourceEngine::ResourceEngine(ResourceSet *resourceSet)
    : QObject(resourceSet), resourceSet(resourceSet), requestId(0)
{
    identifier = (quint32)this;
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
    if(dbus_error_is_set(&dbusError)) {
	qDebug("Error while connection to the system bus: %s", dbusError.message);
	dbus_error_free(&dbusError);
	return false;
    }
    dbus_error_free(&dbusError);
    libresourceConnection = resproto_init(RESPROTO_ROLE_CLIENT, RESPROTO_TRANSPORT_DBUS,
                                           connectionIsUp, dbusConnection);
    if(libresourceConnection == NULL) {
	return NULL;
    }

/*    resproto_set_handler(libresourceConnection, RESMSG_UNREGISTER, handleUnregisterMessage);
    resproto_set_handler(libresourceConnection, RESMSG_GRANT, handleGrantMessage);
    resproto_set_handler(libresourceConnection, RESMSG_ADVICE, handleAdviceMessage);*/
    return true;
}

bool ResourceEngine::connect()
{
    resmsg_t resourceMessage;
    resourceMessage.record.type = RESMSG_REGISTER;
    resourceMessage.record.id = identifier;
    resourceMessage.record.reqno = ++reqestId;

    requestMap.insert(requestId, RESMSG_REGISTER);

    uint32_t allResources, optionalResources, sharedResources;
    allResources = allResourcesToBitmask(resourceSet);
    allResources = optionalResourcesToBitmask(resourceSet);
    allResources = sharedResourcesToBitmask(resourceSet);

    resourceMessage.record.rset.all = allResources;
    resourceMessage.record.rset.opt = optionalResources;
    resourceMessage.record.rset.share = sharedResources;
    resourceMessage.record.rset.mask = 0; //find out what it is

    resourceMessage.record.klass = resourceSet->applicationClass();
    resourceMessage.record.mode = 0; //No auto release

    libresourceSet = resconn_connect(libresourceConnection, &resourceMessage, handleConnect);

    libresourceSet->userdata = this; //save our context
    index = REQHASH_INDEX(reqno);
    reqtyp[index] = RESMSG_REGISTER;
    return true;
}

static inline quint32 allResourcesToBitmask(const ResourceSet *resourceSet)
{
    QList<Resource> resourceList = resourceSet->resources();
    quint32 bitmask=0;
    for(int i=0; i < resourceList.size(); i++) {
        switch(resourceList[i].type()) {
        case AudioPlaybackResource:
            bitmask += RESMSG_AUDIO_PLAYBACK;
            break;
        case VideoPlaybackResource:
            bitmask += RESMSG_VIDEO_PLAYBACK;
            break;
        case AudioRecorderResource:
            bitmask += RESMSG_AUDIO_RECORDING;
            break;
        case VideoRecorderResource:
            bitmask += RESMSG_VIDEO_RECORDING;
            break;
        case VibraResource:
            bitmask += RESMSG_VIBRA;
            break;
        case 
        default:
            break;
        }
    }
    return bitmask;
}

static inline quint32 optionalResourcesToBitmask(const ResourceSet *resourceSet)
{
    QList<Resource> resourceList = resourceSet->resources();
    quint32 bitmask=0;
    for(int i=0; i < resourceList.size(); i++) {
        if(resourceList[i].isOptional()) {
            switch(resourceList[i].type()) {
                case AudioPlaybackResource:
                    bitmask += RESMSG_AUDIO_PLAYBACK;
                    break;
                case VideoPlaybackResource:
                    bitmask += RESMSG_VIDEO_PLAYBACK;
                    break;
                case AudioRecorderResource:
                    bitmask += RESMSG_AUDIO_RECORDING;
                    break;
                case VideoRecorderResource:
                    bitmask += RESMSG_VIDEO_RECORDING;
                    break;
                default:
                    break;
            }
        }
    }
    return bitmask;
}

static inline quint32 sharedResourcesToBitmask(const ResourceSet *resourceSet)
{
    QList<Resource> resourceList = resourceSet->resources();
    quint32 bitmask=0;
    for(int i=0; i < resourceList.size(); i++) {
        if(resourceList[i].isShared()) {
            switch(resourceList[i].type()) {
                case AudioPlaybackResource:
                    bitmask += RESMSG_AUDIO_PLAYBACK;
                    break;
                case VideoPlaybackResource:
                    bitmask += RESMSG_VIDEO_PLAYBACK;
                    break;
                case AudioRecorderResource:
                    bitmask += RESMSG_AUDIO_RECORDING;
                    break;
                case VideoRecorderResource:
                    bitmask += RESMSG_VIDEO_RECORDING;
                    break;
                default:
                    break;
            }
        }
    }
    return bitmask;
}

bool ResourceEngine::isConnected()
{
    return false;
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

    resourceEngine = (ResourceEngine *)(connection->dbus.rsets->userdata);

    resourceEngine->handleConnectionIsUp();
}

void ResourceEngine::handleConnectionIsUp()
{
    emit connectedToManager();
}
