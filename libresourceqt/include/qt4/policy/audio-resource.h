/*************************************************************************
This file is part of libresourceqt

Copyright (C) 2010 Nokia Corporation.

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

#ifndef AUDIO_RESOURCE_H
#define AUDIO_RESOURCE_H

#include <QObject>
#include <QString>
#include <policy/resource.h>

namespace ResourcePolicy
{

/**
 * The AudioResource class represents the audio device. It is a bit 
 * different from other resource types in that in takes more parameters to
 * allow the programmer to classify the audio stream used by the application.
 */
class AudioResource: public QObject, public Resource
{
    Q_OBJECT
public:
	/**
     * The constructor.
     * \param audioGroup The audio group which this application belongs to.
     * This is an optional parameter.
     */
    AudioResource(const QString &audioGroup = QString());
    AudioResource(const AudioResource &other);
    virtual ~AudioResource();

    //! Accessor for the  audioGroup.
    QString audioGroup() const;
    //! A test to check whether the audio group is set or not.
    bool audioGroupIsSet() const;
    /**
     * Set the audio group (classification)
     * \param newGroup The new audio group to set.
     */
    void setAudioGroup(const QString & newGroup);

    //! \return The PID of the process which is responsible for rendering the audio stream.
    quint32 processID() const;
    /**
     * Use this to indicate to the Resource Manager the PID of the audio
     * stream renderer.
     * \param newPID Set this to the PID of the process which will render the audio.
     */
    void setProcessID(quint32 newPID);

    //! \return the name of the stream tag.
    QString streamTagName() const;
    //! \return the value of the stream tag.
    QString streamTagValue() const;
    //! A test to check whether the stream tag has been set or not.
    bool streamTagIsSet() const;
    /**
     * Set the tream tag to help policy to correctly identify the audio stream
     * beloning to you.
     * \param name The name of the tag. For example "media.name"
     * \param value The value of the stream tag.
     */
    void setStreamTag(const QString &name, const QString &value);

    virtual ResourceType type() const;
private:
    QString group;
    quint32 pid;
    QString streamName;
    QString streamValue;
signals:
    void audioPropertiesChanged(const QString &group, quint32 pid,
                                const QString &name, const QString &value);
};
}

#endif
