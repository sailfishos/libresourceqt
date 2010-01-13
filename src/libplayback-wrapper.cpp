#include "libplayback-wrapper.h"

using namespace ResourceTypes;
static inline enum pb_class_e resourceClassToLibPlaybackClass(enum ResourceClass resourceClass);
static inline quint16 resourceFlagsToLibPlaybackFlags(quint16 resourceFlags);
static inline enum ResourceState libPlaybackStateToResourceState(enum pb_state_e libPlaybackState);
static void libPlaybackStateHandler(pb_playback_t *libPlaybackHandler, enum pb_state_e newState,
				    pb_req_t* playbackRequest, void *data);
static void libPlaybackStateHintHandler(pb_playback_t *libPlaybackHandler, const int allowedStates[], void *data);

LibPlaybackWrapper::LibPlaybackWrapper(Resource *res)
    : QObject(res), dbusConnection(NULL), libPlaybackHandle(NULL)
{
    resource = res;
}

LibPlaybackWrapper::~LibPlaybackWrapper()
{
    pb_playback_destroy(libPlaybackHandle);
}

bool LibPlaybackWrapper::initialize()
{
    DBusError dbusError;
    dbus_error_init(&dbusError);
    dbusConnection = dbus_bus_get(DBUS_BUS_SESSION, &dbusError);
    qDebug("dbusConnection=%p", dbusConnection);
    if (dbus_error_is_set(&dbusError)) {
	qDebug("D-Bus Connection Error (%s)\n", dbusError.message);
	dbus_error_free(&dbusError);
	return false;
    }
    if(dbusConnection == NULL) {
	qDebug("Unable to connect to dbus, dbusConnection == NULL!");
	return false;
    }
    return true;
}

bool LibPlaybackWrapper::connectToServer()
{
    enum pb_class_e libPlaybackClass = resourceClassToLibPlaybackClass(resource->applicationClass());
    quint16 libPlaybackFlags = resourceFlagsToLibPlaybackFlags(resource->resources());

    qDebug("libPlaybackFlags=%02x resources=%02x",libPlaybackFlags, resource->resources());
    qDebug("dbusConnection=%p", dbusConnection);

    libPlaybackHandle = pb_playback_new_2(dbusConnection, libPlaybackClass, libPlaybackFlags,
					  PB_STATE_STOP, libPlaybackStateHandler, this);
    if(libPlaybackHandle == NULL) {
	return false;
    }
    return true;
}

inline enum pb_class_e resourceClassToLibPlaybackClass(enum ResourceClass resourceClass)
{
    switch(resourceClass) {
    case InvalidClass:
	return PB_CLASS_NONE;
    case CallClass:
	return PB_CLASS_CALL;
    case MediaClass:
	return PB_CLASS_MEDIA;
    case BackgroundClass:
	return PB_CLASS_BACKGROUND;
    case RingtoneClass:
	return PB_CLASS_RINGTONE;
    case VoiceUIClass:
	return PB_CLASS_VOICEUI;
    case CameraClass:
	return PB_CLASS_CAMERA;
    case GameClass:
	return PB_CLASS_GAME;
    case AlarmClass:
	return PB_CLASS_ALARM;
    case FlashClass:
	return PB_CLASS_FLASH;
    case SystemClass:
	return PB_CLASS_SYSTEM;
    case InputClass:
	return PB_CLASS_INPUT;
    default:
	return PB_CLASS_NONE;
    }
}

inline quint16 resourceFlagsToLibPlaybackFlags(quint16 resourceFlags)
{
    quint16 libPlaybackFlags = 0;
    for (int flag=AudioResource;flag<VideoRecorderResource;flag=flag<<1) {
	if((resourceFlags & flag) == flag) {
	    switch(flag) {
	    case AudioResource:
		libPlaybackFlags += PB_FLAG_AUDIO;
		break;
	    case VideoResource:
		libPlaybackFlags += PB_FLAG_VIDEO;
		break;
	    case AudioRecorderResource:
		libPlaybackFlags += PB_FLAG_AUDIO_RECORDING;
		break;
	    case VideoRecorderResource:
		libPlaybackFlags += PB_FLAG_VIDEO_RECORDING;
		break;
	    }
	}
    }
    return libPlaybackFlags;
}

void libPlaybackStateHandler(pb_playback_t *libPlaybackHandler, enum pb_state_e newState,
			     pb_req_t* playbackRequest, void *data)
{
    LibPlaybackWrapper *libPlaybackWrapper = static_cast<LibPlaybackWrapper*>(data);

    pb_playback_req_completed(libPlaybackHandler, playbackRequest);
    libPlaybackWrapper->stateChanged(newState);
}

void LibPlaybackWrapper::stateChanged(enum pb_state_e newState)
{
    enum ResourceState resourceState;
    resourceState = libPlaybackStateToResourceState(newState);
    
    resource->handleStateChange(resourceState);
}

inline enum ResourceState libPlaybackStateToResourceState(enum pb_state_e libPlaybackState)
{
    switch(libPlaybackState) {
    case PB_STATE_STOP:
	return NotOwnedState;
    case PB_STATE_PLAY:
	return OwnedState;
    default:
	return UnknownState;
    }	
}

static void libPlaybackStateHintHandler(pb_playback_t *libPlaybackHandler, const int allowedStates[], void *data)
{
    LibPlaybackWrapper *libPlaybackWrapper = static_cast<LibPlaybackWrapper*>(data);

    libPlaybackWrapper->hintReceived(allowedStates);
}

void LibPlaybackWrapper::hintReceived(const int allowedStates[])
{
    if(allowedStates[PB_STATE_PLAY]) {
	resource->emitReservable();
    }
    // Ignore PB_STATE_STOP
/*    else if(allowedStates[PB_STATE_STOP]) {
	
    }
*/
}

bool LibPlaybackWrapper::reserve()
{
    return false;
}
