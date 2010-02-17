#include <policy/audio-resource.h>

using namespace ResourcePolicy;

AudioResource::AudioResource(const QString &audioGroup)
        :   QObject(), Resource(), group(audioGroup), pid(0), stream(QString())
{
}

AudioResource::AudioResource(const AudioResource &other)
        :   QObject(), Resource(other), group(other.group), pid(other.pid), stream(other.stream)
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
    qDebug("this = %p", this);
    group = newGroup;
    qDebug() << "Audio group changed! New group is: " << newGroup;
    emit audioGroupChanged(group);
}

quint32 AudioResource::processID() const
{
    return pid;
}

void AudioResource::setProcessID(quint32 newPID)
{
    pid = newPID;
    emit pidChanged(pid);
}

QString AudioResource::streamTag() const
{
    return stream;
}

void AudioResource::setStreamTag(const QString & newStreamTag)
{
    stream = newStreamTag;
    emit streamTagChanged(stream);
}

ResourceType AudioResource::type() const
{
    return AudioPlaybackType;
}

