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
#include "test-resource-set.h"

using namespace ResourcePolicy;

Resource * TestResourceSet::resourceFromType(ResourceType type)
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

TestResourceSet::TestResourceSet()
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

TestResourceSet::~TestResourceSet()
{
}

void TestResourceSet::testConstructor1()
{
    ResourceSet resourceSet("player");
    QVERIFY(resourceSet.isConnectedToManager() == false);
    //QCOMPARE(resourceSet.id(), (quint32) 1);
    QCOMPARE(resourceSet.applicationClass(), QString("player"));
    QVERIFY(resourceSet.willAutoRelease() == false);
    QVERIFY(resourceSet.alwaysGetReply() == false);

    // Test it has no any resources
    QList<Resource*> resources = resourceSet.resources();
    QVERIFY(resources.isEmpty());
    for (int i = 0;i < NumberOfTypes;i++) {
        ResourceType type = (ResourceType)i;
        Resource *res = resourceSet.resource(type);
        QVERIFY(res == NULL);
        bool contains = resourceSet.contains(type);
        QVERIFY(contains == false);
    }
}

void TestResourceSet::testConstructor2()
{
    ResourceSet resourceSet("player", this, true, true);
    QVERIFY(resourceSet.isConnectedToManager() == false);
    //QCOMPARE(resourceSet.id(), (quint32) 1);
    QCOMPARE(resourceSet.applicationClass(), QString("player"));
    QVERIFY(resourceSet.willAutoRelease() == true);
    QVERIFY(resourceSet.alwaysGetReply() == true);

    // Test it has no any resources
    QList<Resource*> resources = resourceSet.resources();
    QVERIFY(resources.isEmpty());
    for (int i = 0;i < NumberOfTypes;i++) {
        ResourceType type = (ResourceType)i;
        Resource *res = resourceSet.resource(type);
        QVERIFY(res == NULL);
        bool contains = resourceSet.contains(type);
        QVERIFY(contains == false);
    }
}

void TestResourceSet::testIdentifier()
{
    ResourceSet resourceSet("player");
    ResourceSet otherSet("game");

    bool identifiersAreUnique = (resourceSet.id() != otherSet.id());
    QVERIFY(identifiersAreUnique);
}

void TestResourceSet::testAddResource()
{
    ResourceSet resourceSet("player");
    for (int i = 0;i < NumberOfTypes;i++) {
        ResourceType type = (ResourceType)i;
        resourceSet.addResource(type);
        bool setContainsGivenResource = resourceSet.contains(type);
        QVERIFY(setContainsGivenResource);
    }
}

void TestResourceSet::testAddResourceObject()
{
    ResourceSet resourceSet("player");
    for (int i = 0;i < NumberOfTypes;i++) {
        ResourceType type = (ResourceType)i;
        Resource *resource = resourceFromType(type);
        resourceSet.addResourceObject(resource);
        bool setContainsGivenResource = resourceSet.contains(type);
        QVERIFY(setContainsGivenResource);
    }
}

void TestResourceSet::testDelResource()
{
    ResourceSet resourceSet("player");
    for (int i = 0;i < NumberOfTypes;i++) {
        ResourceType type = (ResourceType)i;
        resourceSet.addResource(type);
        bool setContainsGivenResource = resourceSet.contains(type);
        QVERIFY(setContainsGivenResource);
    }

    for (int i = 0;i < NumberOfTypes;i++) {
        ResourceType type = (ResourceType)i;
        resourceSet.deleteResource(type);
        bool setNoLongerContainType = !resourceSet.contains(type);
        QVERIFY(setNoLongerContainType);
        for (int j = 0; j < NumberOfTypes; j++) {
            if (j == i)
                continue;
            ResourceType otherType = (ResourceType)j;
            bool setStillContainsOtherTypes = resourceSet.contains(otherType);
            QVERIFY(setStillContainsOtherTypes);
        }
        resourceSet.addResource(type);
    }
}

void TestResourceSet::testContainsSet()
{
    ResourceSet resourceSet("player");
    QList<ResourceType> types, subset;

    for (int i = 0;i < NumberOfTypes;i++) {
        ResourceType type = (ResourceType)i;
        resourceSet.addResource(type);
        types.append(type);
    }

    subset << AudioPlaybackType << VideoPlaybackType
           << AudioRecorderType << VideoRecorderType << LensCoverType;

    bool containsAll = resourceSet.contains(types);
    bool containsSubset = resourceSet.contains(subset);
    QVERIFY(containsAll);
    QVERIFY(containsSubset);
}

void TestResourceSet::testConnectToSignals()
{
    ResourceSet resourceSet("player");
    bool signalConnectionSucceeded=false;
    signalConnectionSucceeded = QObject::connect(&resourceSet,
        SIGNAL(resourcesBecameAvailable(const QList<ResourcePolicy::ResourceType> &)),
        this, SLOT(handleResourcesBecameAvailable(const QList<ResourcePolicy::ResourceType> &)));

    QVERIFY(signalConnectionSucceeded);

    signalConnectionSucceeded = QObject::connect(&resourceSet,
        SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)),
        this, SLOT(handleResourcesGranted(const QList<ResourcePolicy::ResourceType> &)));

    QVERIFY(signalConnectionSucceeded);

    signalConnectionSucceeded = QObject::connect(&resourceSet,
        SIGNAL(resourcesDenied()),
        this, SLOT(handleResourcesDenied()));

    QVERIFY(signalConnectionSucceeded);

    signalConnectionSucceeded = QObject::connect(&resourceSet,
        SIGNAL(resourcesReleased()),
        this, SLOT(handleResourcesReleased()));

    QVERIFY(signalConnectionSucceeded);

    signalConnectionSucceeded = QObject::connect(&resourceSet,
        SIGNAL(lostResources()),
        this, SLOT(handleLostResources()));

    QVERIFY(signalConnectionSucceeded);
}

void TestResourceSet::handleResourcesBecameAvailable(const QList<ResourcePolicy::ResourceType> &)
{
}

void TestResourceSet::handleResourcesGranted(const QList<ResourcePolicy::ResourceType> &)
{
}

void TestResourceSet::handleResourcesDenied()
{
}

void TestResourceSet::handleResourcesReleased()
{
}

void TestResourceSet::handleLostResources()
{
}

void TestResourceSet::testConnectEngine()
{
    ResourceSet resourceSet("player");
    bool connectOk = resourceSet.initAndConnect();
    QVERIFY(connectOk);
}

void TestResourceSet::testConnectEngine2()
{
    ResourceSet resourceSet("player");
    bool addOk = resourceSet.addResource(AudioPlaybackType);
    QVERIFY(addOk);
    bool connectOk = resourceSet.initAndConnect();
    QVERIFY(connectOk);
}

void TestResourceSet::waitForSignal(const QObject *sender, const char *signal, quint32 timeout)
{
    QEventLoop loop;
    QTimer::singleShot(timeout, &loop, SLOT(quit()));
    loop.connect(sender, signal, SLOT(quit()));
    loop.exec();
}

void TestResourceSet::testAcquire()
{
    ResourceSet resourceSet("player");

    // Test that signals gets emitted
    QSignalSpy stateSpy(&resourceSet,
            SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QVERIFY(stateSpy.isValid());
    QSignalSpy stateSpy2(&resourceSet, SIGNAL(resourcesReleased()));
    QVERIFY(stateSpy2.isValid());

    bool addOk = resourceSet.addResource(AudioPlaybackType);
    QVERIFY(addOk);
    bool connectOk = resourceSet.initAndConnect();
    QVERIFY(connectOk);
    bool acquireOk = resourceSet.acquire();
    QVERIFY(acquireOk);
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));

    // Check the signal got received
    QCOMPARE(stateSpy.count(), 1);

    bool releaseOk = resourceSet.release();
    QVERIFY(releaseOk);
    waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));

    // Check the signal got received
    QCOMPARE(stateSpy2.count(), 1);
}

// Test that there are no signals emitted after acquire() and release() are
// done again.
void TestResourceSet::testDoubleAcquire()
{
    ResourceSet resourceSet("player");

    // Test that signals gets emitted
    QSignalSpy stateSpy(&resourceSet,
            SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QVERIFY(stateSpy.isValid());
    QSignalSpy stateSpy2(&resourceSet, SIGNAL(resourcesReleased()));
    QVERIFY(stateSpy2.isValid());

    bool addOk = resourceSet.addResource(AudioPlaybackType);
    QVERIFY(addOk);
    bool connectOk = resourceSet.initAndConnect();
    QVERIFY(connectOk);
    bool acquireOk = resourceSet.acquire();
    QVERIFY(acquireOk);
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));

    // Check the signal got received
    QCOMPARE(stateSpy.count(), 1);

    acquireOk = resourceSet.acquire();
    QVERIFY(acquireOk);
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));

    // Check there is no second signal
    QCOMPARE(stateSpy.count(), 1);

    bool releaseOk = resourceSet.release();
    QVERIFY(releaseOk);
    waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));

    // Check the signal got received
    QCOMPARE(stateSpy2.count(), 1);

    releaseOk = resourceSet.release();
    QVERIFY(releaseOk);
    waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));

    // Check there is no second signal
    QCOMPARE(stateSpy2.count(), 1);
}

QTEST_MAIN(TestResourceSet)
