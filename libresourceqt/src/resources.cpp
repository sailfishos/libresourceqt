#include <policy/resources.h>

using namespace ResourcePolicy;

AudioRecorderResource::AudioRecorderResource()
        :   Resource()
{
}

AudioRecorderResource::AudioRecorderResource(const AudioRecorderResource &other)
        :   Resource(other), group(other.group), pid(other.pid), stream(other.stream)
{
}

AudioRecorderResource::~AudioRecorderResource()
{
}

ResourceType AudioRecorderResource::type() const
{
    return AudioRecorderType;
}

VideoResource::VideoResource()
        :   Resource()
{
}

VideoResource::VideoResource(const VideoResource &other)
        :    Resource(other)
{
}

VideoResource::~VideoResource()
{
}

ResourceType VideoResource::type() const
{
    return VideoPlaybackType;
}

VideoRecorderResource::VideoRecorderResource()
        :   Resource()
{
}

VideoRecorderResource::VideoRecorderResource(const VideoRecorderResource &other)
        :    Resource(other)
{
}

VideoRecorderResource::~VideoRecorderResource()
{
}

ResourceType VideoRecorderResource::type() const
{
    return VideoRecorderType;
}

VibraResource::VibraResource()
        :   Resource()
{
}

VibraResource::VibraResource(const VibraResource &other)
        :    Resource(other)
{
}

VibraResource::~VibraResource()
{
}

ResourceType VibraResource::type() const
{
    return VibraType;
}

LedsResource::LedsResource()
        :   Resource()
{
}

LedsResource::LedsResource(const LedsResource &other)
        :    Resource(other)
{
}

LedsResource::~LedsResource()
{
}

ResourceType LedsResource::type() const
{
    return LedsType;
}

BacklightResource::BacklightResource()
        :   Resource()
{
}

BacklightResource::BacklightResource(const BacklightResource &other)
        :    Resource(other)
{
}

BacklightResource::~BacklightResource()
{
}

ResourceType BacklightResource::type() const
{
    return BacklightType;
}

SystemButtonResource::SystemButtonResource()
        :   Resource()
{
}

SystemButtonResource::SystemButtonResource(const SystemButtonResource &other)
        :    Resource(other)
{
}

SystemButtonResource::~SystemButtonResource()
{
}

ResourceType SystemButtonResource::type() const
{
    return SystemButtonType;
}

LockButtonResource::LockButtonResource()
        :   Resource()
{
}

LockButtonResource::LockButtonResource(const LockButtonResource &other)
        :    Resource(other)
{
}

LockButtonResource::~LockButtonResource()
{
}

ResourceType LockButtonResource::type() const
{
    return LockButtonType;
}

ScaleButtonResource::ScaleButtonResource()
        :   Resource()
{
}

ScaleButtonResource::ScaleButtonResource(const ScaleButtonResource &other)
        :    Resource(other)
{
}

ScaleButtonResource::~ScaleButtonResource()
{
}

ResourceType ScaleButtonResource::type() const
{
    return ScaleButtonType;
}

SnapButtonResource::SnapButtonResource()
        :   Resource()
{
}

SnapButtonResource::SnapButtonResource(const SnapButtonResource &other)
        :    Resource(other)
{
}

SnapButtonResource::~SnapButtonResource()
{
}

ResourceType SnapButtonResource::type() const
{
    return SnapButtonType;
}

LensCoverResource::LensCoverResource()
        :   Resource()
{
}

LensCoverResource::LensCoverResource(const LensCoverResource &other)
        :    Resource(other)
{
}

LensCoverResource::~LensCoverResource()
{
}

ResourceType LensCoverResource::type() const
{
    return LensCoverType;
}
