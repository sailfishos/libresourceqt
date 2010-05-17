#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>

#include "client.h"

using namespace ResourcePolicy;

Client::Client(int testCnt, QString appClass, uint32_t all, uint32_t optional) : QObject()
{
    QSettings settings("/usr/lib/libresourceqt-performance-tests/test-resourceqt-performance.ini", QSettings::IniFormat);

    settings.beginGroup("main");
    reportAcquire = settings.value("acqreport", "/tmp/acquire.csv").toString();
    reportRelease = settings.value("relreport", "/tmp/release.csv").toString();
    settings.endGroup();

    totalTestCount = testCnt;
    applicationClass = appClass;
    resourceSet = NULL;
    testState = TestStart;
    testCounter = 1;

    allResources = all;
    optionalResources = optional;
    firstTime = true;

    failed = 0;

    mainTimerID = startTimer(0);
}

Client::~Client()
{
    killTimer(mainTimerID);

    if( resourceSet != NULL )
    {
        delete resourceSet;
        resourceSet = NULL;
    }
}

uint32_t Client::parseResourceList(QString resourceListStr)
{
    struct
    {
        uint32_t resourceType;
        const char* resourceName;
    }
    resourceDef[] =
    {
        { RES_AUDIO_PLAYBACK, "AudioPlayback" },
        { RES_VIDEO_PLAYBACK, "VideoPlayback" },
        { RES_AUDIO_RECORDING, "AudioRecording" },
        { RES_VIDEO_RECORDING, "VideoRecording" },
        { RES_VIBRA, "Vibra" },
        { RES_LEDS, "Leds" },
        { RES_BACKLIGHT, "BackLight" },
        { RES_SYSTEM_BUTTON, "SystemButton" },
        { RES_LOCK_BUTTON, "LockButton" },
        { RES_SCALE_BUTTON, "ScaleButton" },
        { RES_SNAP_BUTTON, "SnapButton" },
        { RES_LENS_COVER, "LensCover" },
        { 0, NULL }
    };

    uint32_t resourceList = 0;

    if( resourceListStr.isEmpty() || resourceListStr.isNull() )
    {
        return 0;
    }
    else
    {
        QStringList resList = resourceListStr.split(",", QString::SkipEmptyParts);

        for( int i = 0; i < resList.count(); i++ )
        {
            int pos = 0;
            while( resourceDef[pos].resourceName != NULL )
            {
                if( resList[i] == resourceDef[pos].resourceName )
                    break;

                pos++;
            }

            if( !resourceDef[pos].resourceName )
            {
                const char* res = qPrintable(resList[i]);
                printf("Ignoring invalid resource name '%s'\n", res);
            }
            else
            {
                resourceList |= resourceDef[pos].resourceType;
            }
        }
    }

    return resourceList;
}

void Client::createSet(uint32_t list, uint32_t optional)
{
    uint32_t resources[] =
    {
        RES_AUDIO_PLAYBACK, RES_VIDEO_PLAYBACK, RES_AUDIO_RECORDING, RES_VIDEO_RECORDING, RES_VIBRA, RES_LEDS, RES_BACKLIGHT, RES_SYSTEM_BUTTON,
        RES_LOCK_BUTTON, RES_SCALE_BUTTON, RES_SNAP_BUTTON, RES_LENS_COVER, 0
    };

    int pos = 0;
    while( resources[pos] )
    {
        if( list & resources[pos] )
        {
            Resource* resource = NULL;
            ResourceType res = getResourceType(resources[pos]);
            bool opt = (optional & resources[pos]) == resources[pos];

            resource = allocateResource(res, opt);
            if( resource )
            {
                resourceSet->addResourceObject(resource);
            }
        }

        pos++;
    }
}

bool Client::initialize()
{
    if( resourceSet )
    {
        return false;
    }

    resourceSet = new ResourceSet(applicationClass);
    if( resourceSet == NULL )
    {
        return false;
    }
    resourceSet->setAlwaysReply();

    createSet(allResources, optionalResources);

    if( !connect(resourceSet, SIGNAL(resourcesGranted(QList<ResourcePolicy::ResourceType> )), this, SLOT(resourceAcquiredHandler(QList<ResourcePolicy::ResourceType>))) )
    {
        return false;
    }

    if( !connect(resourceSet, SIGNAL(resourcesDenied()), this, SLOT(resourceDeniedHandler())) )
    {
        return false;
    }

    if( !connect(resourceSet, SIGNAL(lostResources()), this, SLOT(resourceLostHandler())) )
    {
        return false;
    }

    connect(resourceSet, SIGNAL(resourcesReleased()), this, SLOT(resourceReleasedHandler()));

    return true;
}

bool Client::cleanup()
{
    if( !disconnect(resourceSet, SIGNAL(resourcesGranted(QList<ResourcePolicy::ResourceType> )), this, SLOT(resourceAcquiredHandler(QList<ResourcePolicy::ResourceType>))) )
    {
        return false;
    }

    if( !disconnect(resourceSet, SIGNAL(resourcesDenied()), this, SLOT(resourceDeniedHandler())) )
    {
        return false;
    }

    if( !disconnect(resourceSet, SIGNAL(lostResources()), this, SLOT(resourceLostHandler())) )
    {
        return false;
    }

    disconnect(resourceSet, SIGNAL(resourcesReleased()), this, SLOT(resourceReleasedHandler()));

    if( resourceSet != NULL )
    {
        delete resourceSet;
        resourceSet = NULL;
    }

    return true;
}

Resource* Client::allocateResource(ResourceType resource, bool optional)
{
    Resource* retValue = NULL;

    switch( resource )
    {
    case AudioPlaybackType:
        retValue = new AudioResource();
        break;
    case VideoPlaybackType:
        retValue = new VideoResource();
        break;
    case AudioRecorderType:
        retValue = new AudioRecorderResource();
        break;
    case VideoRecorderType:
        retValue = new VideoRecorderResource();
        break;
    case VibraType:
        retValue = new VibraResource();
        break;
    case LedsType:
        retValue = new LedsResource();
        break;
    case BacklightType:
        retValue = new BacklightResource();
        break;
    case SystemButtonType:
        retValue = new SystemButtonResource();
        break;
    case LockButtonType:
        retValue = new LockButtonResource();
        break;
    case ScaleButtonType:
        retValue = new ScaleButtonResource();
        break;
    case SnapButtonType:
        retValue = new SnapButtonResource();
        break;
    case LensCoverType:
        retValue = new LensCoverResource();
        break;
    case NumberOfTypes:
        return NULL;
    }

    if( retValue )
    {
        retValue->setOptional(optional);
    }
    else
    {
        printf("Unknown resource type - %d\n", resource);
    }

    return retValue;
}

ResourceType Client::getResourceType(uint32_t resource)
{
    switch( resource )
    {
    case RES_AUDIO_PLAYBACK:
        return AudioPlaybackType;
    case RES_VIDEO_PLAYBACK:
        return VideoPlaybackType;
    case RES_AUDIO_RECORDING:
        return AudioRecorderType;
    case RES_VIDEO_RECORDING:
        return VideoRecorderType;
    case RES_VIBRA:
        return VibraType;
    case RES_LEDS:
        return LedsType;
    case RES_BACKLIGHT:
        return BacklightType;
    case RES_SYSTEM_BUTTON:
        return SystemButtonType;
    case RES_LOCK_BUTTON:
        return LockButtonType;
    case RES_SCALE_BUTTON:
        return ScaleButtonType;
    case RES_SNAP_BUTTON:
        return SnapButtonType;
    case RES_LENS_COVER:
        return LensCoverType;
    }

    return NumberOfTypes;
}

void Client::resourceAcquiredHandler(const QList<ResourceType>& /*grantedResList*/)
{
    timeStatAcq.markEnd(false);
    testState = TestRelease;
}

void Client::resourceDeniedHandler()
{
/*    if( timeStat.markEnd() )
    {
        timeStat.report();
    }*/
}

void Client::resourceLostHandler()
{
/*    if( timeStat.markEnd() )
    {
        timeStat.report();
    }*/
}

void Client::resourceReleasedHandler()
{
    timeStatRel.markEnd(false);
    testState = TestFinished;
}

void Client::timerEvent(QTimerEvent*)
{
    if( firstTime )
    {
        firstTime = false;
        timeStatTotal.markStart();
        initialize();
    }

    switch( testState )
    {
    case TestFinished:
        if( testCounter == totalTestCount )
        {
            cleanup();
            timeStatTotal.markEnd(false);
            puts("\n*** RESULTS ***");
            printf("Total failed: %d/%d\n", failed, totalTestCount);
            printf("Total time  : %.2f ms\n", timeStatTotal.caseTime);
            printf("Average time: %.2f ms (%.2f ms)\n", timeStatTotal.caseTime / (double)totalTestCount, timeStatTotal.caseTime / (double)(totalTestCount*2));
            printf("Acquire:\n\tAverage: %.2f ms\n\tMin    : %.2f ms\n\tMax    : %.2f ms\n", timeStatAcq.totalTime / (double)timeStatAcq.caseCount, timeStatAcq.totalMin, timeStatAcq.totalMax);
            printf("Release:\n\tAverage: %.2f ms\n\tMin    : %.2f ms\n\tMax    : %.2f ms\n", timeStatRel.totalTime / (double)timeStatRel.caseCount, timeStatRel.totalMin, timeStatRel.totalMax);
            timeStatAcq.exportData(qPrintable(reportAcquire));
            timeStatRel.exportData(qPrintable(reportRelease));
            QMetaObject::invokeMethod(QCoreApplication::instance(), "quit");
        }
        else
        {
            testCounter++;
            testState = TestStart;
        }
        break;
    case TestWaiting:
        break;
    case TestStart:
        testState = TestAcquire;
        printf("\rRunning round %d of %d ... ", testCounter, totalTestCount);
        fflush(stdout);
        break;
    case TestAcquire:
        timeStatAcq.markStart();
        if( !resourceSet || !resourceSet->acquire() )
        {
            timeStatAcq.markEnd(true);
            testState = TestFinished;
            failed++;
        }
        else
            testState = TestWaiting;
        break;
    case TestRelease:
        timeStatRel.markStart();
        if( !resourceSet || !resourceSet->release() )
        {
            timeStatRel.markEnd(true);
            testState = TestFinished;
            failed++;
        }
        else
            testState = TestWaiting;
        break;
    }
}
