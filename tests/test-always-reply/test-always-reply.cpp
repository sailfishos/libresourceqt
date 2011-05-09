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
#include "test-always-reply.h"

using namespace ResourcePolicy;

TestAlwaysReply::TestAlwaysReply()
{
}

TestAlwaysReply::~TestAlwaysReply()
{
}

void TestAlwaysReply::waitForSignal(const QObject *sender, const char *signal, quint32 timeout)
{
    QEventLoop loop;
    QTimer::singleShot(timeout, &loop, SLOT(quit()));
    loop.connect(sender, signal, SLOT(quit()));
    loop.exec();
}

// This test tests rejecting resource without alwaysReply flag
void TestAlwaysReply::testNoAlwaysReply()
{
    ResourceSet resourceSet("alarm", this, false, false);
    ResourceSet resourceSet2("player", this, false, false);

    // Install signal watchers
    QSignalSpy stateSpyGranted(&resourceSet,
            SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QVERIFY(stateSpyGranted.isValid());
    QSignalSpy stateSpyLost(&resourceSet, SIGNAL(lostResources()));
    QVERIFY(stateSpyLost.isValid());
    QSignalSpy stateSpyReleased(&resourceSet, SIGNAL(resourcesReleased()));
    QVERIFY(stateSpyReleased.isValid());
    QSignalSpy stateSpyDenied(&resourceSet, SIGNAL(resourcesDenied()));
    QVERIFY(stateSpyDenied.isValid());
    QSignalSpy stateSpyUpdateOK(&resourceSet, SIGNAL(updateOK()));
    QVERIFY(stateSpyUpdateOK.isValid());
    QSignalSpy stateSpyResourcesReleased(&resourceSet, SIGNAL(resourcesReleasedByManager()));
    QVERIFY(stateSpyResourcesReleased.isValid());

    QSignalSpy stateSpyGranted2(&resourceSet2,
            SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QVERIFY(stateSpyGranted2.isValid());
    QSignalSpy stateSpyLost2(&resourceSet2, SIGNAL(lostResources()));
    QVERIFY(stateSpyLost2.isValid());
    QSignalSpy stateSpyReleased2(&resourceSet2, SIGNAL(resourcesReleased()));
    QVERIFY(stateSpyReleased2.isValid());
    QSignalSpy stateSpyDenied2(&resourceSet2, SIGNAL(resourcesDenied()));
    QVERIFY(stateSpyDenied2.isValid());
    QSignalSpy stateSpyUpdateOK2(&resourceSet2, SIGNAL(updateOK()));
    QVERIFY(stateSpyUpdateOK2.isValid());
    QSignalSpy stateSpyResourcesReleased2(&resourceSet2, SIGNAL(resourcesReleasedByManager()));
    QVERIFY(stateSpyResourcesReleased2.isValid());

    // Create resource sets
    bool addOk = resourceSet.addResource(AudioPlaybackType);
    QVERIFY(addOk);
    bool connectOk = resourceSet.initAndConnect();
    QVERIFY(connectOk);
    bool addOk2 = resourceSet2.addResource(AudioPlaybackType);
    QVERIFY(addOk2);
    bool connectOk2 = resourceSet2.initAndConnect();
    QVERIFY(connectOk2);

    // Acquire the resource for the first client
    bool acquireOk = resourceSet.acquire();
    QVERIFY(acquireOk);
    // Wait for the granted-signal for the first client
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted.count(), 1);

    // Acquire the resource for the second client
    bool acquireOk2 = resourceSet2.acquire();
    QVERIFY(acquireOk2);
    // The resource is not granted but there is no signaling when there is no
    // alwaysReply flag

    // Wait for a while so that the signals would come
    QTest::qWait(200);
    QCOMPARE(stateSpyGranted2.count(), 0);

    // Release the resource from the first client
    bool releaseOk = resourceSet.release();
    QVERIFY(releaseOk);
    // Wait for the released-signal
    waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased.count(), 1);

    // Check there was no granted- or denied-signals for second client
    //QCOMPARE(stateSpyGranted2.count(), 0);
    QCOMPARE(stateSpyDenied2.count(), 0);
    QCOMPARE(stateSpyReleased2.count(), 0);

    // Now the second client gets the resource
    // Wait for the granted-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted2.count(), 1);
    QCOMPARE(stateSpyLost.count(), 0);
    QCOMPARE(stateSpyDenied2.count(), 0);

    // Release the resource from the second client
    bool releaseOk2 = resourceSet2.release();
    QVERIFY(releaseOk2);
    // Wait for the released-signal
    waitForSignal(&resourceSet2, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased2.count(), 1);

    // Check all the signals
    QCOMPARE(stateSpyGranted.count(), 1);
    QCOMPARE(stateSpyLost.count(), 0);
    QCOMPARE(stateSpyReleased.count(), 1);
    QCOMPARE(stateSpyDenied.count(), 0);
    QCOMPARE(stateSpyUpdateOK.count(), 0);
    QCOMPARE(stateSpyResourcesReleased.count(), 0);

    QCOMPARE(stateSpyGranted2.count(), 1);
    QCOMPARE(stateSpyLost2.count(), 0);
    QCOMPARE(stateSpyReleased2.count(), 1);
    QCOMPARE(stateSpyDenied2.count(), 0);
    QCOMPARE(stateSpyUpdateOK2.count(), 0);
    QCOMPARE(stateSpyResourcesReleased2.count(), 0);
}

// This test tests rejecting resource with alwaysReply flag
void TestAlwaysReply::testAlwaysReply()
{
    ResourceSet resourceSet("alarm", this, true, false);
    ResourceSet resourceSet2("player", this, true, false);

    // Install signal watchers
    QSignalSpy stateSpyGranted(&resourceSet,
            SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QVERIFY(stateSpyGranted.isValid());
    QSignalSpy stateSpyLost(&resourceSet, SIGNAL(lostResources()));
    QVERIFY(stateSpyLost.isValid());
    QSignalSpy stateSpyReleased(&resourceSet, SIGNAL(resourcesReleased()));
    QVERIFY(stateSpyReleased.isValid());
    QSignalSpy stateSpyDenied(&resourceSet, SIGNAL(resourcesDenied()));
    QVERIFY(stateSpyDenied.isValid());
    QSignalSpy stateSpyUpdateOK(&resourceSet, SIGNAL(updateOK()));
    QVERIFY(stateSpyUpdateOK.isValid());
    QSignalSpy stateSpyResourcesReleased(&resourceSet, SIGNAL(resourcesReleasedByManager()));
    QVERIFY(stateSpyResourcesReleased.isValid());
    QSignalSpy stateSpyBecameAvailable(&resourceSet, SIGNAL(resourcesBecameAvailable(const QList<ResourcePolicy::ResourceType> &)));
    QVERIFY(stateSpyBecameAvailable.isValid());

    QSignalSpy stateSpyGranted2(&resourceSet2,
            SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QVERIFY(stateSpyGranted2.isValid());
    QSignalSpy stateSpyLost2(&resourceSet2, SIGNAL(lostResources()));
    QVERIFY(stateSpyLost2.isValid());
    QSignalSpy stateSpyReleased2(&resourceSet2, SIGNAL(resourcesReleased()));
    QVERIFY(stateSpyReleased2.isValid());
    QSignalSpy stateSpyDenied2(&resourceSet2, SIGNAL(resourcesDenied()));
    QVERIFY(stateSpyDenied2.isValid());
    QSignalSpy stateSpyUpdateOK2(&resourceSet2, SIGNAL(updateOK()));
    QVERIFY(stateSpyUpdateOK2.isValid());
    QSignalSpy stateSpyResourcesReleased2(&resourceSet2, SIGNAL(resourcesReleasedByManager()));
    QVERIFY(stateSpyResourcesReleased2.isValid());
    QSignalSpy stateSpyBecameAvailable2(&resourceSet2, SIGNAL(resourcesBecameAvailable(const QList<ResourcePolicy::ResourceType> &)));
    QVERIFY(stateSpyBecameAvailable2.isValid());

    // Create resource sets
    bool addOk = resourceSet.addResource(AudioPlaybackType);
    QVERIFY(addOk);
    bool connectOk = resourceSet.initAndConnect();
    QVERIFY(connectOk);
    bool addOk2 = resourceSet2.addResource(AudioPlaybackType);
    QVERIFY(addOk2);
    bool connectOk2 = resourceSet2.initAndConnect();
    QVERIFY(connectOk2);

    // Acquire the resource for the first client
    bool acquireOk = resourceSet.acquire();
    QVERIFY(acquireOk);
    // Wait for the granted-signal for the first client
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted.count(), 1);

    // Acquire the resource for the second client
    bool acquireOk2 = resourceSet2.acquire();
    QVERIFY(acquireOk2);

    // Wait for the denied-signal
    waitForSignal(&resourceSet2, SIGNAL(resourcesDenied()));
    QCOMPARE(stateSpyGranted2.count(), 0);
    QCOMPARE(stateSpyDenied2.count(), 1);

    // Release the resource from the first client
    bool releaseOk = resourceSet.release();
    QVERIFY(releaseOk);
    // Wait for the released-signal
    waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased.count(), 1);

    // Now the second client gets the resource
    // Wait for the granted-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyBecameAvailable2.count(), 3);
    QCOMPARE(stateSpyGranted2.count(), 1);
    QCOMPARE(stateSpyLost.count(), 0);
    QCOMPARE(stateSpyDenied2.count(), 1);

    // Release the resource from the second client
    bool releaseOk2 = resourceSet2.release();
    QVERIFY(releaseOk2);
    // Wait for the released-signal
    waitForSignal(&resourceSet2, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased2.count(), 1);

    // Check all the signals
    QCOMPARE(stateSpyGranted.count(), 1);
    QCOMPARE(stateSpyLost.count(), 0);
    QCOMPARE(stateSpyReleased.count(), 1);
    QCOMPARE(stateSpyDenied.count(), 0);
    QCOMPARE(stateSpyUpdateOK.count(), 0);
    QCOMPARE(stateSpyResourcesReleased.count(), 0);
    QCOMPARE(stateSpyBecameAvailable.count(), 1);

    QCOMPARE(stateSpyGranted2.count(), 1);
    QCOMPARE(stateSpyLost2.count(), 0);
    QCOMPARE(stateSpyReleased2.count(), 1);
    QCOMPARE(stateSpyDenied2.count(), 1);
    QCOMPARE(stateSpyUpdateOK2.count(), 0);
    QCOMPARE(stateSpyResourcesReleased2.count(), 0);
    QCOMPARE(stateSpyBecameAvailable2.count(), 3);
}

// This test tests simple acquire with two clients
void TestAlwaysReply::testAlwaysReply2()
{
    ResourceSet resourceSet("alarm", this, true, false);
    ResourceSet resourceSet2("alarm", this, true, false);

    // Install signal watchers
    QSignalSpy stateSpyGranted(&resourceSet,
            SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QVERIFY(stateSpyGranted.isValid());
    QSignalSpy stateSpyLost(&resourceSet, SIGNAL(lostResources()));
    QVERIFY(stateSpyLost.isValid());
    QSignalSpy stateSpyReleased(&resourceSet, SIGNAL(resourcesReleased()));
    QVERIFY(stateSpyReleased.isValid());
    QSignalSpy stateSpyDenied(&resourceSet, SIGNAL(resourcesDenied()));
    QVERIFY(stateSpyDenied.isValid());
    QSignalSpy stateSpyUpdateOK(&resourceSet, SIGNAL(updateOK()));
    QVERIFY(stateSpyUpdateOK.isValid());
    QSignalSpy stateSpyResourcesReleased(&resourceSet, SIGNAL(resourcesReleasedByManager()));
    QVERIFY(stateSpyResourcesReleased.isValid());
    QSignalSpy stateSpyBecameAvailable(&resourceSet, SIGNAL(resourcesBecameAvailable(const QList<ResourcePolicy::ResourceType> &)));
    QVERIFY(stateSpyBecameAvailable.isValid());

    QSignalSpy stateSpyGranted2(&resourceSet2,
            SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QVERIFY(stateSpyGranted2.isValid());
    QSignalSpy stateSpyLost2(&resourceSet2, SIGNAL(lostResources()));
    QVERIFY(stateSpyLost2.isValid());
    QSignalSpy stateSpyReleased2(&resourceSet2, SIGNAL(resourcesReleased()));
    QVERIFY(stateSpyReleased2.isValid());
    QSignalSpy stateSpyDenied2(&resourceSet2, SIGNAL(resourcesDenied()));
    QVERIFY(stateSpyDenied2.isValid());
    QSignalSpy stateSpyUpdateOK2(&resourceSet2, SIGNAL(updateOK()));
    QVERIFY(stateSpyUpdateOK2.isValid());
    QSignalSpy stateSpyResourcesReleased2(&resourceSet2, SIGNAL(resourcesReleasedByManager()));
    QVERIFY(stateSpyResourcesReleased2.isValid());
    QSignalSpy stateSpyBecameAvailable2(&resourceSet2, SIGNAL(resourcesBecameAvailable(const QList<ResourcePolicy::ResourceType> &)));
    QVERIFY(stateSpyBecameAvailable2.isValid());

    // Create resource sets
    bool addOk = resourceSet.addResource(AudioPlaybackType);
    QVERIFY(addOk);
    bool connectOk = resourceSet.initAndConnect();
    QVERIFY(connectOk);
    bool addOk2 = resourceSet2.addResource(AudioPlaybackType);
    QVERIFY(addOk2);
    bool connectOk2 = resourceSet2.initAndConnect();
    QVERIFY(connectOk2);

    // Acquire the resource for the first client
    QCOMPARE(stateSpyBecameAvailable.count(), 0);
    QCOMPARE(stateSpyBecameAvailable2.count(), 0);
    bool acquireOk = resourceSet.acquire();
    QVERIFY(acquireOk);
    // Wait for the granted-signal for the first client
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted.count(), 1);
    QCOMPARE(stateSpyBecameAvailable.count(), 1);

    // Acquire the resource for the second client
    bool acquireOk2 = resourceSet2.acquire();
    QVERIFY(acquireOk2);
    // Wait for the lost-signal for the first client
    // The lost-signal for the first client must become before the granted-
    // signal for the second client
    waitForSignal(&resourceSet, SIGNAL(lostResources()));
    QCOMPARE(stateSpyGranted2.count(), 0);
    QCOMPARE(stateSpyLost.count(), 1);
    // Wait for the granted-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted.count(), 1);
    QCOMPARE(stateSpyGranted2.count(), 1);
    QCOMPARE(stateSpyBecameAvailable2.count(), 1);

    // Release the resource from the second client
    bool releaseOk2 = resourceSet2.release();
    QVERIFY(releaseOk2);
    // Wait for the released-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased2.count(), 1);
    QCOMPARE(stateSpyGranted.count(), 2);

    // Wait for the resourcesBecameAvailable-signal for the first client
    waitForSignal(&resourceSet, SIGNAL(resourcesBecameAvailable(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyBecameAvailable.count(), 1);

    // Acquire the resource for the first client
    QCOMPARE(stateSpyGranted.count(), 2);
    acquireOk = resourceSet.acquire();
    QVERIFY(acquireOk);
    // Wait for the granted-signal for the first client
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted.count(), 3);

    // Release the resource from the first client
    bool releaseOk = resourceSet.release();
    QVERIFY(releaseOk);
    // Wait for the released-signal
    waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased.count(), 1);

    // Check all the signals
    QCOMPARE(stateSpyGranted.count(), 3);
    QCOMPARE(stateSpyLost.count(), 1);
    QCOMPARE(stateSpyReleased.count(), 1);
    QCOMPARE(stateSpyDenied.count(), 0);
    QCOMPARE(stateSpyUpdateOK.count(), 0);
    QCOMPARE(stateSpyResourcesReleased.count(), 0);
    QCOMPARE(stateSpyBecameAvailable.count(), 1);

    QCOMPARE(stateSpyGranted2.count(), 1);
    QCOMPARE(stateSpyLost2.count(), 0);
    QCOMPARE(stateSpyReleased2.count(), 1);
    QCOMPARE(stateSpyDenied2.count(), 0);
    QCOMPARE(stateSpyUpdateOK2.count(), 0);
    QCOMPARE(stateSpyResourcesReleased2.count(), 0);
    QCOMPARE(stateSpyBecameAvailable2.count(), 1);
}

// Test that there are new signals emitted after acquire() and release() are
// done again.
void TestAlwaysReply::testDoubleAcquire()
{
    ResourceSet resourceSet("player", this, true, false);

    // Install signal watchers
    QSignalSpy stateSpyGranted(&resourceSet,
            SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QVERIFY(stateSpyGranted.isValid());
    QSignalSpy stateSpyLost(&resourceSet, SIGNAL(lostResources()));
    QVERIFY(stateSpyLost.isValid());
    QSignalSpy stateSpyReleased(&resourceSet, SIGNAL(resourcesReleased()));
    QVERIFY(stateSpyReleased.isValid());
    QSignalSpy stateSpyDenied(&resourceSet, SIGNAL(resourcesDenied()));
    QVERIFY(stateSpyDenied.isValid());
    QSignalSpy stateSpyUpdateOK(&resourceSet, SIGNAL(updateOK()));
    QVERIFY(stateSpyUpdateOK.isValid());
    QSignalSpy stateSpyResourcesReleased(&resourceSet, SIGNAL(resourcesReleasedByManager()));
    QVERIFY(stateSpyResourcesReleased.isValid());
    QSignalSpy stateSpyBecameAvailable(&resourceSet, SIGNAL(resourcesBecameAvailable(const QList<ResourcePolicy::ResourceType> &)));
    QVERIFY(stateSpyBecameAvailable.isValid());

    bool addOk = resourceSet.addResource(AudioPlaybackType);
    QVERIFY(addOk);
    bool connectOk = resourceSet.initAndConnect();
    QVERIFY(connectOk);
    bool acquireOk = resourceSet.acquire();
    QVERIFY(acquireOk);
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));

    // Check the signal got received
    QCOMPARE(stateSpyGranted.count(), 1);

    acquireOk = resourceSet.acquire();
    QVERIFY(acquireOk);
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));

    // Check the second signal got received
    QCOMPARE(stateSpyGranted.count(), 2);

    bool releaseOk = resourceSet.release();
    QVERIFY(releaseOk);
    waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased.count(), 1);

    // Check the signal got received
    QCOMPARE(stateSpyGranted.count(), 2);

    releaseOk = resourceSet.release();
    QVERIFY(releaseOk);
    waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased.count(), 2);

    // Check all the signals
    QCOMPARE(stateSpyGranted.count(), 2);
    QCOMPARE(stateSpyLost.count(), 0);
    QCOMPARE(stateSpyReleased.count(), 2);
    QCOMPARE(stateSpyDenied.count(), 0);
    QCOMPARE(stateSpyUpdateOK.count(), 0);
    QCOMPARE(stateSpyResourcesReleased.count(), 0);
    QCOMPARE(stateSpyBecameAvailable.count(), 1);
}

void TestAlwaysReply::testUpdate()
{
    ResourceSet resourceSet("player", this, true, false);

    // Install signal watchers
    QSignalSpy stateSpyGranted(&resourceSet,
            SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QVERIFY(stateSpyGranted.isValid());
    QSignalSpy stateSpyLost(&resourceSet, SIGNAL(lostResources()));
    QVERIFY(stateSpyLost.isValid());
    QSignalSpy stateSpyReleased(&resourceSet, SIGNAL(resourcesReleased()));
    QVERIFY(stateSpyReleased.isValid());
    QSignalSpy stateSpyDenied(&resourceSet, SIGNAL(resourcesDenied()));
    QVERIFY(stateSpyDenied.isValid());
    QSignalSpy stateSpyUpdateOK(&resourceSet, SIGNAL(updateOK()));
    QVERIFY(stateSpyUpdateOK.isValid());
    QSignalSpy stateSpyResourcesReleased(&resourceSet, SIGNAL(resourcesReleasedByManager()));
    QVERIFY(stateSpyResourcesReleased.isValid());
    QSignalSpy stateSpyBecameAvailable(&resourceSet, SIGNAL(resourcesBecameAvailable(const QList<ResourcePolicy::ResourceType> &)));
    QVERIFY(stateSpyBecameAvailable.isValid());

    bool addOk = resourceSet.addResource(AudioPlaybackType);
    QVERIFY(addOk);
    bool connectOk = resourceSet.initAndConnect();
    QVERIFY(connectOk);

    bool acquireOk = resourceSet.acquire();
    QVERIFY(acquireOk);
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));

    // Check the granted signal was received
    QCOMPARE(stateSpyGranted.count(), 1);

    bool releaseOk = resourceSet.release();
    QVERIFY(releaseOk);
    waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));

    // Check the released signal was received
    QCOMPARE(stateSpyReleased.count(), 1);

    addOk = resourceSet.addResource(VideoPlaybackType);
    QVERIFY(addOk);
    bool updateOk = resourceSet.update();
    QVERIFY(updateOk);
    waitForSignal(&resourceSet, SIGNAL(updateOK()));

    // Check the updateOK signal was received
    QCOMPARE(stateSpyUpdateOK.count(), 1);
    QCOMPARE(stateSpyReleased.count(), 1);
    QCOMPARE(stateSpyGranted.count(), 1);

    // Check all the signals
    QCOMPARE(stateSpyGranted.count(), 1);
    QCOMPARE(stateSpyLost.count(), 0);
    QCOMPARE(stateSpyReleased.count(), 1);
    QCOMPARE(stateSpyDenied.count(), 0);
    QCOMPARE(stateSpyUpdateOK.count(), 1);
    QCOMPARE(stateSpyResourcesReleased.count(), 0);
    QCOMPARE(stateSpyBecameAvailable.count(), 1);
}

void TestAlwaysReply::testUpdateGranted()
{
    ResourceSet resourceSet("player", this, true, false);

    // Test that signals gets emitted
    // Install signal watchers
    QSignalSpy stateSpyGranted(&resourceSet,
            SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QVERIFY(stateSpyGranted.isValid());
    QSignalSpy stateSpyLost(&resourceSet, SIGNAL(lostResources()));
    QVERIFY(stateSpyLost.isValid());
    QSignalSpy stateSpyReleased(&resourceSet, SIGNAL(resourcesReleased()));
    QVERIFY(stateSpyReleased.isValid());
    QSignalSpy stateSpyDenied(&resourceSet, SIGNAL(resourcesDenied()));
    QVERIFY(stateSpyDenied.isValid());
    QSignalSpy stateSpyUpdateOK(&resourceSet, SIGNAL(updateOK()));
    QVERIFY(stateSpyUpdateOK.isValid());
    QSignalSpy stateSpyResourcesReleased(&resourceSet, SIGNAL(resourcesReleasedByManager()));
    QVERIFY(stateSpyResourcesReleased.isValid());
    QSignalSpy stateSpyBecameAvailable(&resourceSet, SIGNAL(resourcesBecameAvailable(const QList<ResourcePolicy::ResourceType> &)));
    QVERIFY(stateSpyBecameAvailable.isValid());

    bool addOk = resourceSet.addResource(AudioPlaybackType);
    QVERIFY(addOk);
    bool connectOk = resourceSet.initAndConnect();
    QVERIFY(connectOk);

    bool acquireOk = resourceSet.acquire();
    QVERIFY(acquireOk);
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));

    // Check the granted signal was received
    QCOMPARE(stateSpyGranted.count(), 1);

    addOk = resourceSet.addResource(VideoPlaybackType);
    QVERIFY(addOk);
    bool updateOk = resourceSet.update();
    QVERIFY(updateOk);
    waitForSignal(&resourceSet, SIGNAL(updateOK()));

    // Check the updateOK signal was not received and granted-signal was
    // received
    QCOMPARE(stateSpyUpdateOK.count(), 0);
    QCOMPARE(stateSpyReleased.count(), 0);
    QCOMPARE(stateSpyGranted.count(), 2);

    bool releaseOk = resourceSet.release();
    QVERIFY(releaseOk);
    waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));

    // Check the released signal was received
    QCOMPARE(stateSpyReleased.count(), 1);

    // Check all the signals
    QCOMPARE(stateSpyGranted.count(), 2);
    QCOMPARE(stateSpyLost.count(), 0);
    QCOMPARE(stateSpyReleased.count(), 1);
    QCOMPARE(stateSpyDenied.count(), 0);
    QCOMPARE(stateSpyUpdateOK.count(), 0);
    QCOMPARE(stateSpyResourcesReleased.count(), 0);
    QCOMPARE(stateSpyBecameAvailable.count(), 2);
}

QTEST_MAIN(TestAlwaysReply)
