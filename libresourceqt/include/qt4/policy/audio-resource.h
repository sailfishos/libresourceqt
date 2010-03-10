#ifndef AUDIO_RESOURCE_H
#define AUDIO_RESOURCE_H

#include <QObject>
#include <QString>
#include <policy/resource.h>

namespace ResourcePolicy
{

class AudioResource: public QObject, public Resource
{
    Q_OBJECT
public:
    AudioResource(const QString &audioGroup = QString());
    AudioResource(const AudioResource &other);
    virtual ~AudioResource();

    QString audioGroup() const;
    bool audioGroupIsSet() const;
    /**
     * Set the audio group (classification)
     * \param newGroup The new audio group to set.
     */
    void setAudioGroup(const QString & newGroup);

    quint32 processID() const;
    /**
     * Set the PID of the process which will render the audio. Use this if the
     * audio renderer is in a separate process.
     */
    void setProcessID(quint32 newPID);

    QString streamTagName() const;
    QString streamTagValue() const;
    bool streamTagIsSet() const;
    /**
     * Set the tream tag to help policy to correctly identify the audio stream
     * beloning to you
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
