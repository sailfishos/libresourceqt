#ifndef RESOURCE_TYPES_H
#define RESOURCE_TYPES_H

//! This namespace contains the enumerations used by the Resource library.
namespace ResourceTypes {

   //! This enum represents the class of the application which uses the resource.
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

   //! This enum defines the different resources that an application can request.
   //! \note These are bits and can be combined to a bit mask.
   enum ResourceType {
	   AudioResource = 0x01,
	   VideoResource = 0x02,
	   AudioRecorderResource = 0x04,
	   VideoRecorderResource = 0x08
   };

   //! The current state of a resource.
   enum ResourceState {
	   UnknownState,
	   NotOwnedState,
	   OwnedState
   };
}
#endif
