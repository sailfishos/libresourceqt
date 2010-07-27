#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <QObject>
#include <QtCore/QTextStream>

#include <stdint.h>
#include <policy/resource-set.h>
#include <sys/resource.h>

#define RES_AUDIO_PLAYBACK	(1<<0)
#define RES_VIDEO_PLAYBACK	(1<<1)
#define RES_AUDIO_RECORDING	(1<<2)
#define RES_VIDEO_RECORDING	(1<<3)
#define RES_VIBRA			(1<<4)
#define RES_LEDS			(1<<5)
#define RES_BACKLIGHT		(1<<6)
#define RES_SYSTEM_BUTTON	(1<<7)
#define RES_LOCK_BUTTON		(1<<8)
#define RES_SCALE_BUTTON	(1<<9)
#define RES_SNAP_BUTTON		(1<<10)
#define RES_LENS_COVER		(1<<11)
#define RES_HEADSET_BUTTONS (1<<12)

class TimeStat
{
public:
    double totalTime;

    TimeStat()
    {
        running = false;
        totalTime = 0;
    };

    inline void markStart()
    {
        running = true;
        getrusage(RUSAGE_SELF, &start);
    }

    inline bool markEnd()
    {
        getrusage(RUSAGE_SELF, &end);

        if( !running )
            return false;

        running = false;
        timevalSub(&end.ru_utime, &start.ru_utime, &end.ru_utime);
        timevalSub(&end.ru_stime, &start.ru_stime, &end.ru_stime);

        double sys = end.ru_stime.tv_sec * 1000.0 + end.ru_stime.tv_usec / 1000.0;
        double usr = end.ru_utime.tv_sec * 1000.0 + end.ru_utime.tv_usec / 1000.0;
        totalTime = sys + usr;

        return true;
    }

    void report(const char* format)
    {
        printf(format, totalTime);
    }

private:
    bool            running;
    struct rusage   start;
    struct rusage   end;

    void timevalSub(struct timeval *a, struct timeval *b, struct timeval *diff)
    {
        diff->tv_sec = a->tv_sec - b->tv_sec;
        if( a->tv_usec < b->tv_usec )
        {
            diff->tv_sec--;
            diff->tv_usec = 1000000 - b->tv_usec + a->tv_usec;
        }
        else
            diff->tv_usec = a->tv_usec - b->tv_usec;
    }
};

class Client : public QObject
{
	Q_OBJECT

public:
	Client(QString appClass);
	~Client();

	bool initialize(uint32_t all, uint32_t optional, bool alwaysReply, bool autoRelease);
	static uint32_t parseResourceList(QString resourceListStr);

private slots:
	void resourceAcquiredHandler(const QList<ResourcePolicy::ResourceType>& grantedResList);
	void resourceDeniedHandler();
	void resourceLostHandler();
	void resourceReleasedHandler();
	void resourcesBecameAvailableHandler(const QList<ResourcePolicy::ResourceType> &availableResources);

protected:
    void timerEvent(QTimerEvent *e);

private:
    QTextStream*	standardInput;
    int				mainTimerID;

	uint32_t		resourcesAll;
	uint32_t		resourcesOptional;
	QString			applicationClass;

	TimeStat        timeStat;

	ResourcePolicy::ResourceSet* 	resourceSet;

	ResourcePolicy::Resource* allocateResource(ResourcePolicy::ResourceType resource, bool optional);
	ResourcePolicy::ResourceType getResourceType(uint32_t resource);

	void showPrompt();
	void showResources(const QList<ResourcePolicy::ResourceType> resList);
	void showResources(const QList<ResourcePolicy::Resource*> resList);
	void updateSet(uint32_t list, uint32_t optional, bool remove);
};

#endif
