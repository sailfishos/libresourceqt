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

Resource * AudioRecorderResource::clone() const
{
    return new AudioRecorderResource(*this);
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

Resource * VideoResource::clone() const
{
    return new VideoResource(*this);
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

Resource * VideoRecorderResource::clone() const
{
    return new VideoRecorderResource(*this);
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

Resource * VibraResource::clone() const
{
    return new VibraResource(*this);
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

Resource * LedsResource::clone() const
{
    return new LedsResource(*this);
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

Resource * BacklightResource::clone() const
{
    return new BacklightResource(*this);
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

Resource * SystemButtonResource::clone() const
{
    return new SystemButtonResource(*this);
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

Resource * LockButtonResource::clone() const
{
    return new LockButtonResource(*this);
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

Resource * ScaleButtonResource::clone() const
{
    return new ScaleButtonResource(*this);
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

Resource * SnapButtonResource::clone() const
{
    return new SnapButtonResource(*this);
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

Resource * LensCoverResource::clone() const
{
    return new LensCoverResource(*this);
}

LensCoverResource::~LensCoverResource()
{
}

ResourceType LensCoverResource::type() const
{
    return LensCoverType;
}

