/*************************************************************************
This file is part of libresourceqt

Copyright (C) 2011 Nokia Corporation.

This library is free software; you can redistribute
it and/or modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation
version 2.1 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
USA.
*************************************************************************/

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
    group = newGroup;
    emit audioPropertiesChanged(group, pid, streamName, streamValue);
}

quint32 AudioResource::processID() const
{
    return pid;
}

void AudioResource::setProcessID(quint32 newPID)
{
    pid = newPID;
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
