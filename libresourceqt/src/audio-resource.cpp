#include <policy/audio-resource.h>

using namespace ResourcePolicy;

AudioResource::AudioResource(const QString &audioGroup)
        :   QObject(), Resource(), group(audioGroup), pid(0),
        streamName(QString()), streamValue(QString())
{
}

AudioResource::AudioResource(const AudioResource &other)
        :   QObject(), Resource(other), group(other.group), pid(other.pid),
        streamName(other.streamName), streamValue(other.streamValue)
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

bool AudioResource::audioGroupIsSet() const
{
    if (group.isEmpty() || group.isNull()) {
        return false;
    }
    return true;
}

void AudioResource::setAudioGroup(const QString &newGroup)
{
    qDebug("this = %p", this);
    group = newGroup;
    qDebug() << "Audio group changed! New group is: " << newGroup;
    emit audioPropertiesChanged(group, pid, streamName, streamValue);
}

quint32 AudioResource::processID() const
{
    return pid;
}

void AudioResource::setProcessID(quint32 newPID)
{
    pid = newPID;
    qDebug("New PID = %u", pid);
    emit audioPropertiesChanged(group, pid, streamName, streamValue);
}

QString AudioResource::streamTagName() const
{
    return streamName;
}

QString AudioResource::streamTagValue() const
{
    return streamValue;
}

bool AudioResource::streamTagIsSet() const
{
    if (streamName.isEmpty() || streamName.isNull() ||
        streamValue.isEmpty() || streamValue.isNull()) {
        return false;
    }
    return true;
}

void AudioResource::setStreamTag(const QString &name, const QString &value)
{
    streamName = name;
    streamValue = value;
    emit audioPropertiesChanged(group, pid, name, value);
}

ResourceType AudioResource::type() const
{
    return AudioPlaybackType;
}

