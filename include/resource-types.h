#ifndef RESOURCE_TYPES_H
#define RESOURCE_TYPES_H

namespace ResourceTypes {

   enum ResourceClass {
	   InvalidClass = 0,
	   VoIPClass,
	   CallClass = VoIPClass,
	   MediaClass,
	   BackgroundClass,
	   RingtoneClass,
	   VoiceUIClass,
	   CameraClass,
	   GameClass,
	   AlarmClass,
	   FlashClass,
	   SystemClass,
	   InputClass
   };

   enum ResourceType {
	   AudioResource = 0x01,
	   VideoResource = 0x02,
	   AudioRecorderResource = 0x04,
	   VideoRecorderResource = 0x08
   };

   enum ResourceState {
	   UnknownState,
	   NotOwnedState,
	   OwnedState
   };
}
#endif
