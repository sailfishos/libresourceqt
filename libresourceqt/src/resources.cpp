#include "resources.h"

using namespace ResourcePolicy;

AudioResource::AudioResource(const QString &audioGroup)
    :   Resource(), group(audioGroup), pid(0), stream(QString())
{
}

AudioResource::AudioResource(const AudioResource &other)
    :   Resource(other), group(other.group), pid(other.pid), stream(other.stream)
{
}

Resource * AudioResource::clone() const
{
    return new AudioResource(*this);
}

AudioResource::~AudioResource()
{
}

QString AudioResource::audioGroup() const
{
    return group;
}

void AudioResource::setAudioGroup(const QString &newGroup)
{
    group = newGroup;
}

quint32 AudioResource::processID() const
{
    return pid;
}

void AudioResource::setProcessID(quint32 newPID)
{
    pid = newPID;
}

QString AudioResource::streamTag() const
{
    return stream;
}

void AudioResource::setStreamTag(const QString & newStreamTag)
{
    stream = newStreamTag;
}

ResourceType AudioResource::type() const
{
    return AudioPlaybackType;
}

AudioRecorderResource::AudioRecorderResource(const QString & audioGroup)
    :   Resource(), group(audioGroup), pid(0), stream(QString())
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

void AudioRecorderResource::setAudioGroup(const QString & newGroup)
{
    group = newGroup;
}

quint32 AudioRecorderResource::processID() const
{
    return pid;
}

void AudioRecorderResource::setProcessID(quint32 newPID)
{
    pid = newPID;
}

QString AudioRecorderResource::streamTag() const
{
    return stream;
}

void AudioRecorderResource::setStreamTag(const QString & newStreamTag)
{
    stream = newStreamTag;
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

