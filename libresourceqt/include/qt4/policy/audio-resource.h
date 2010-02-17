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
    void setAudioGroup(const QString & newGroup);

    quint32 processID() const;
    void setProcessID(quint32 newPID);

    QString streamTag() const;
    void setStreamTag(const QString & newStreamTag);

    virtual ResourceType type() const;
    virtual Resource * clone() const;
private:
    QString group;
    quint32 pid;
    QString stream;
signals:
    void pidChanged(quint32 newPid);
    void audioGroupChanged(const QString &newGroup);
    void streamTagChanged(const QString &newTag);
};
}

#endif

