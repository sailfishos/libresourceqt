#ifndef RESOURCES_H
#define RESOURCES_H

#include <policy/resource.h>
#include <QObject>
#include <QString>

namespace ResourcePolicy
{

class AudioResource: public Resource
{
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
};

class AudioRecorderResource: public Resource
{
public:
    AudioRecorderResource(const QString & audioGroup = QString());
    AudioRecorderResource(const AudioRecorderResource &other);
    virtual ~AudioRecorderResource();

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
};

class BacklightResource: public Resource
{
public:
    BacklightResource();
    BacklightResource(const BacklightResource &other);
    virtual ~BacklightResource();

    virtual ResourceType type() const;
    virtual Resource * clone() const;
};

class LedsResource: public Resource
{
public:
    LedsResource();
    LedsResource(const LedsResource &other);
    virtual ~LedsResource();

    virtual ResourceType type() const;
    virtual Resource * clone() const;
};

class VibraResource: public Resource
{
public:
    VibraResource();
    VibraResource(const VibraResource &other);
    virtual ~VibraResource();

    virtual ResourceType type() const;
    virtual Resource * clone() const;
};

class VideoRecorderResource: public Resource
{
public:
    VideoRecorderResource();
    VideoRecorderResource(const VideoRecorderResource &other);
    virtual ~VideoRecorderResource();

    virtual ResourceType type() const;
    virtual Resource * clone() const;
};

class VideoResource: public Resource
{
public:
    VideoResource();
    VideoResource(const VideoResource &other);
    virtual ~VideoResource();

    virtual ResourceType type() const;
    virtual Resource * clone() const;
};

class SystemButtonResource: public Resource
{
public:
    SystemButtonResource();
    SystemButtonResource(const SystemButtonResource &other);
    virtual ~SystemButtonResource();

    virtual ResourceType type() const;
    virtual Resource * clone() const;
};

class LockButtonResource: public Resource
{
public:
    LockButtonResource();
    LockButtonResource(const LockButtonResource &other);
    virtual ~LockButtonResource();

    virtual ResourceType type() const;
    virtual Resource * clone() const;
};

class ScaleButtonResource: public Resource
{
public:
    ScaleButtonResource();
    ScaleButtonResource(const ScaleButtonResource &other);
    virtual ~ScaleButtonResource();

    virtual ResourceType type() const;
    virtual Resource * clone() const;
};

class SnapButtonResource: public Resource
{
public:
    SnapButtonResource();
    SnapButtonResource(const SnapButtonResource &other);
    virtual ~SnapButtonResource();

    virtual ResourceType type() const;
    virtual Resource * clone() const;
};

class LensCoverResource: public Resource
{
public:
    LensCoverResource();
    LensCoverResource(const LensCoverResource &other);
    virtual ~LensCoverResource();

    virtual ResourceType type() const;
    virtual Resource * clone() const;
};
}
#endif

