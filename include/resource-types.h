#ifndef RESOURCE_TYPES_H
#define RESOURCE_TYPES_H
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

#define RP_FLAGS_AUDIO 0x01
#define RP_FLAGS_VIDEO 0x02
#define RP_FLAGS_AUDIO_RECORDING 0x04
#define RP_FLAGS_VIDEO_RECORDING 0x08

#endif
