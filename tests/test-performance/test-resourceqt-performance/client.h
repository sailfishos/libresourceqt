#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <QList>
#include <QObject>

#include <policy/resource-set.h>
#include <sys/time.h>

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

class TimeStat
{
public:
    double totalTime;
    double caseTime;
    double totalMax;
    double totalMin;
    int    caseCount;

    TimeStat()
    {
        running = false;
        totalTime = 0;
        caseTime = 0;
        caseCount = 0;
        totalMax = 0;
        totalMin = 9999999999.;
    };

    inline void markStart()
    {
        if( running )
            return;

        running = true;
        gettimeofday(&start, NULL);
    }

    inline void markEnd(bool failed)
    {
        if( !running )
            return;

        gettimeofday(&end, NULL);

        running = false;

        timevalSub(&end, &start, &end);
        caseTime = end.tv_sec * 1000.0 + end.tv_usec / 1000.0;

        if( !failed )
        {
            totalMax = (totalMax < caseTime) ? caseTime : totalMax;
            totalMin = (totalMin > caseTime) ? caseTime : totalMin;
            totalTime += caseTime;
            data.push_back(caseTime);
            caseCount++;
        }
    }

    void exportData(const char* name)
    {
        FILE* f = fopen(name, "wt");

        fprintf(f, "Cnt;%d\n", caseCount);
        fprintf(f, "Tot;%.4f\n", totalTime);
        fprintf(f, "Max;%.4f\n", totalMax);
        fprintf(f, "Min;%.4f\n", totalMin);
        fprintf(f, "Avg;%.4f\n", totalTime / (double)caseCount);
        int pos = 0;
        DataList::iterator it;
        for( it = data.begin(); it != data.end(); ++it )
        {
            fprintf(f, "%d;%.4f\n", ++pos, *it);
        }
        fclose(f);
    }

private:
    typedef QList<double>   DataList;

    DataList        data;
    bool            running;
    struct timeval  start;
    struct timeval  end;


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
   static uint32_t parseResourceList(QString resourceListStr);

public:
	Client(int testCnt, QString appClass, uint32_t all, uint32_t optional);
	~Client();

	bool initialize();
	bool cleanup();
	int getFailed() { return failed; }

	double getAcquireAverage() { return timeStatAcq.totalTime / (double)timeStatAcq.caseCount; }
    double getReleaseAverage() { return timeStatRel.totalTime / (double)timeStatRel.caseCount; }

private slots:
	void resourceAcquiredHandler(const QList<ResourcePolicy::ResourceType>& grantedResList);
	void resourceDeniedHandler();
	void resourceLostHandler();
	void resourceReleasedHandler();

protected:
    void timerEvent(QTimerEvent *e);

private:
    enum TestState
    {
        TestFinished = 0,
        TestStart,
        TestAcquire,
        TestRelease,
        TestWaiting
    };

    QString         reportAcquire;
    QString         reportRelease;
    TestState       testState;
    int             testCounter;
    int				mainTimerID;
	QString			applicationClass;
	TimeStat        timeStatAcq;
    TimeStat        timeStatRel;
    TimeStat        timeStatTotal;
    bool            firstTime;

	uint32_t        allResources;
	uint32_t        optionalResources;

	int             failed;
	int             totalTestCount;

	ResourcePolicy::ResourceSet* 	resourceSet;

	ResourcePolicy::Resource* allocateResource(ResourcePolicy::ResourceType resource, bool optional);
	ResourcePolicy::ResourceType getResourceType(uint32_t resource);

	void createSet(uint32_t list, uint32_t optional);
};

#endif
