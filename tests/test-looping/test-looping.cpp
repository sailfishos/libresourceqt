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

#include <QSignalSpy>
#include <QList>
#include <QEventLoop>
#include <QTimer>
#include "test-looping.h"

using namespace ResourcePolicy;

Resource * TestLooping::resourceFromType(ResourceType type)
{
    switch (type) {
    case AudioPlaybackType:
        return new AudioResource;
    case AudioRecorderType:
        return new AudioRecorderResource;
    case VideoPlaybackType:
        return new VideoResource;
    case VideoRecorderType:
        return new VideoRecorderResource;
    case VibraType:
        return new VibraResource;
    case LedsType:
        return new LedsResource;
    case BacklightType:
        return new BacklightResource;
    case SystemButtonType:
        return new SystemButtonResource;
    case LockButtonType:
        return new LockButtonResource;
    case ScaleButtonType:
        return new ScaleButtonResource;
    case SnapButtonType:
        return new SnapButtonResource;
    case LensCoverType:
        return new LensCoverResource;
    case HeadsetButtonsType:
        return new HeadsetButtonsResource;
    default:
        return NULL;
    }
}

using namespace ResourcePolicy;

TestLooping::TestLooping()
    : audioResource(NULL)
    , audioRecorderResource(NULL)
    , videoResource(NULL)
    , videoRecorderResource(NULL)
    , vibraResource(NULL)
    , ledsResource(NULL)
    , backlightResource(NULL)
    , systemButtonResource(NULL)
    , lockButtonResource(NULL)
    , scaleButtonResource(NULL)
    , snapButtonResource(NULL)
    , lensCoverResource(NULL)
    , headsetButtonsResource(NULL)
{
}

TestLooping::~TestLooping()
{
}

void TestLooping::waitForSignal(const QObject *sender, const char *signal, quint32 timeout)
{
    QEventLoop loop;
    QTimer::singleShot(timeout, &loop, SLOT(quit()));
    loop.connect(sender, signal, SLOT(quit()));
    loop.exec();
}

void TestLooping::loopAcquireSend()
{
    ResourceSet resourceSet("player");

    QSignalSpy stateSpyGranted(&resourceSet,
            SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QVERIFY(stateSpyGranted.isValid());
    QSignalSpy stateSpyReleased(&resourceSet, SIGNAL(resourcesReleased()));
    QVERIFY(stateSpyReleased.isValid());

    resourceSet.addResource(AudioPlaybackType);
    resourceSet.initAndConnect();
    for (int i = 0; i < 10000; i++) {
        bool acquireOk = resourceSet.acquire();
        QVERIFY(acquireOk);
    }
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    // Wait for more possible signals..
    QTest::qWait(1000);
    QCOMPARE(stateSpyGranted.count(), 1);

    bool releaseOk = resourceSet.release();
    QVERIFY(releaseOk);
    waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased.count(), 1);
}

void TestLooping::loopAcquireSend2()
{
    ResourceSet resourceSet("player");

    QSignalSpy stateSpyGranted(&resourceSet,
            SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QVERIFY(stateSpyGranted.isValid());
    QSignalSpy stateSpyReleased(&resourceSet, SIGNAL(resourcesReleased()));
    QVERIFY(stateSpyReleased.isValid());

    resourceSet.addResource(AudioPlaybackType);
    resourceSet.initAndConnect();
    for (int i = 0; i < 10000; i++) {
        bool acquireOk = resourceSet.acquire();
        QVERIFY(acquireOk);

        bool releaseOk = resourceSet.release();
        QVERIFY(releaseOk);
    }
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));
    // Wait for more possible signals..
    QTest::qWait(1000);
    QCOMPARE(stateSpyGranted.count(), 1);
    QCOMPARE(stateSpyReleased.count(), 1);
}

/*
void BenchmarkResourceSet::benchmarkReleaseSend()
{
    ResourceSet resourceSet("player");
    resourceSet.addResource(AudioPlaybackType);
    resourceSet.initAndConnect();
    resourceSet.acquire();
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));

    QBENCHMARK {
        resourceSet.release();
    }
    waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));
}

void BenchmarkResourceSet::benchmarkAcquire()
{
    ResourceSet resourceSet("player");
    resourceSet.addResource(AudioPlaybackType);
    resourceSet.initAndConnect();
    QBENCHMARK {
        resourceSet.acquire();
        waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    }

    resourceSet.release();
    waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));
}

void BenchmarkResourceSet::benchmarkRelease()
{
    ResourceSet resourceSet("player");
    resourceSet.addResource(AudioPlaybackType);
    resourceSet.initAndConnect();
    resourceSet.acquire();
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));

    QBENCHMARK {
        resourceSet.release();
        waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));
    }
}
*/
QTEST_MAIN(TestLooping)
