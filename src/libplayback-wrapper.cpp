#include "libplayback-wrapper.h"

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
    dbusConnection = dbus_bus_get(DBUS_BUS_SESSION, &dbusError);
    if(dbusConnection == NULL) {
	return false;
    }
    
    enum pb_class_e libPlaybackClass = resourceClassToLibPlaybackClass(resource->getResourceClass());
    quint16 libPlaybackFlags = resourceFlagToLibPlaybackFlags(resource->getResourceFlags());
    libPlaybackHandle = pb_playback_new_2(dbusConnection, libPlaybackClass, libPlaybackFlags,
					  PB_STATE_STOP, libPlaybackStateHandler, this);
    if(libPlaybackHandle == NULL) {
	return false;
    }
    return true;
}

enum pb_class_e resourceClassToLibPlaybackClass(enum ResourceClass resourceClass)
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

quint16 resourceFlagToLibPlaybackFlags(quint16 resourceFlags)
{
    quint16 libPlaybackFlags = 0;
    for (int flag=RP_FLAGS_AUDIO;flag<RP_FLAGS_VIDEO_RECORDING;flag=flag<<1) {
	if((resourceFlags & flag) == flag) {
	    switch(flag) {
	    case RP_FLAGS_AUDIO:
		libPlaybackFlags += PB_FLAG_AUDIO;
		break;
	    case RP_FLAGS_VIDEO:
		libPlaybackFlags += PB_FLAG_VIDEO;
		break;
	    case RP_FLAGS_AUDIO_RECORDING:
		libPlaybackFlags += PB_FLAG_AUDIO_RECORDING;
		break;
	    case RP_FLAGS_VIDEO_RECORDING:
		libPlaybackFlags += PB_FLAG_VIDEO_RECORDING;
		break;
	    }
	}
    }
    return libPlaybackFlags;
}

void libPlaybackStateHandler(pb_playback_t *libPlaybackHandle, enum pb_state_e requestedState,
			     pb_req_t* playbackRequest, void *data)
{
//    LibPlaybackWrapper *libPlaybackWrapper = (LibPlaybackWrapper*)data;

//    libPlaybackWrapper->handleNewState(requestedState);

    pb_playback_req_completed(libPlaybackHandle, playbackRequest);
}
