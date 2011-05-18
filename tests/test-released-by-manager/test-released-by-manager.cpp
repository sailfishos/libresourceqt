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
#include "test-released-by-manager.h"

using namespace ResourcePolicy;

TestReleasedByManager::TestReleasedByManager()
{
}

TestReleasedByManager::~TestReleasedByManager()
{
}

void TestReleasedByManager::waitForSignal(const QObject *sender, const char *signal, quint32 timeout)
{
    QEventLoop loop;
    QTimer::singleShot(timeout, &loop, SLOT(quit()));
    loop.connect(sender, signal, SLOT(quit()));
    loop.exec();
}

// This test tests simple acquire with two clients
void TestReleasedByManager::testLostNoPlayer()
{
    ResourceSet resourceSet("alarm");
    ResourceSet resourceSet2("alarm");

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
    // Wait for the lost-signal for the first client
    // The lost-signal for the first client must become before the granted-
    // signal for the second client
    waitForSignal(&resourceSet, SIGNAL(lostResources()));
    //QCOMPARE(stateSpyGranted2.count(), 0);
    QCOMPARE(stateSpyLost.count(), 1);
    // Wait for the granted-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted2.count(), 1);

    // Release the resource from the second client
    bool releaseOk2 = resourceSet2.release();
    QVERIFY(releaseOk2);
    // Wait for the released-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased2.count(), 1);
    // Wait for the granted-signal for the first client
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted.count(), 2);

    // Release the resource from the first client
    bool releaseOk = resourceSet.release();
    QVERIFY(releaseOk);
    // Wait for the released-signal
    waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased.count(), 1);

    // Check all the signals
    QCOMPARE(stateSpyGranted.count(), 2);
    QCOMPARE(stateSpyLost.count(), 1);
    QCOMPARE(stateSpyReleased.count(), 1);
    QCOMPARE(stateSpyDenied.count(), 0);
    QCOMPARE(stateSpyUpdateOK.count(), 0);
    QCOMPARE(stateSpyResourcesReleased.count(), 0);
    QCOMPARE(stateSpyBecameAvailable.count(), 3);

    QCOMPARE(stateSpyGranted2.count(), 1);
    QCOMPARE(stateSpyLost2.count(), 0);
    QCOMPARE(stateSpyReleased2.count(), 1);
    QCOMPARE(stateSpyDenied2.count(), 0);
    QCOMPARE(stateSpyUpdateOK2.count(), 0);
    QCOMPARE(stateSpyResourcesReleased2.count(), 0);
    QCOMPARE(stateSpyBecameAvailable2.count(), 1);
}

// This test tests simple acquire with two clients
void TestReleasedByManager::testLostFirstPlayer()
{
    ResourceSet resourceSet("player");
    ResourceSet resourceSet2("alarm");

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
    // Wait for the lost-signal for the first client
    // The lost-signal for the first client must become before the granted-
    // signal for the second client
    waitForSignal(&resourceSet, SIGNAL(lostResources()));
    //QCOMPARE(stateSpyGranted2.count(), 0);
    QCOMPARE(stateSpyLost.count(), 1);
    // Wait for the granted-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted2.count(), 1);

    // Release the resource from the second client
    bool releaseOk2 = resourceSet2.release();
    QVERIFY(releaseOk2);
    // Wait for the released-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased2.count(), 1);
    // Wait for the granted-signal for the first client
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted.count(), 2);

    // Release the resource from the first client
    bool releaseOk = resourceSet.release();
    QVERIFY(releaseOk);
    // Wait for the released-signal
    waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased.count(), 1);

    // Check all the signals
    QCOMPARE(stateSpyGranted.count(), 2);
    QCOMPARE(stateSpyLost.count(), 1);
    QCOMPARE(stateSpyReleased.count(), 1);
    QCOMPARE(stateSpyDenied.count(), 0);
    QCOMPARE(stateSpyUpdateOK.count(), 0);
    QCOMPARE(stateSpyResourcesReleased.count(), 0);
    QCOMPARE(stateSpyBecameAvailable.count(), 3);

    QCOMPARE(stateSpyGranted2.count(), 1);
    QCOMPARE(stateSpyLost2.count(), 0);
    QCOMPARE(stateSpyReleased2.count(), 1);
    QCOMPARE(stateSpyDenied2.count(), 0);
    QCOMPARE(stateSpyUpdateOK2.count(), 0);
    QCOMPARE(stateSpyResourcesReleased2.count(), 0);
    QCOMPARE(stateSpyBecameAvailable2.count(), 1);
}

// This test tests simple acquire with two clients
void TestReleasedByManager::testLostSecondPlayer()
{
    ResourceSet resourceSet("background");
    ResourceSet resourceSet2("player");

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
    // Wait for the lost-signal for the first client
    // The lost-signal for the first client must become before the granted-
    // signal for the second client
    waitForSignal(&resourceSet, SIGNAL(lostResources()));
    //QCOMPARE(stateSpyGranted2.count(), 0);
    QCOMPARE(stateSpyLost.count(), 1);
    // Wait for the granted-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted2.count(), 1);

    // Release the resource from the second client
    bool releaseOk2 = resourceSet2.release();
    QVERIFY(releaseOk2);
    // Wait for the released-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased2.count(), 1);
    // Wait for the granted-signal for the first client
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted.count(), 2);

    // Release the resource from the first client
    bool releaseOk = resourceSet.release();
    QVERIFY(releaseOk);
    // Wait for the released-signal
    waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased.count(), 1);

    // Check all the signals
    QCOMPARE(stateSpyGranted.count(), 2);
    QCOMPARE(stateSpyLost.count(), 1);
    QCOMPARE(stateSpyReleased.count(), 1);
    QCOMPARE(stateSpyDenied.count(), 0);
    QCOMPARE(stateSpyUpdateOK.count(), 0);
    QCOMPARE(stateSpyResourcesReleased.count(), 0);
    QCOMPARE(stateSpyBecameAvailable.count(), 3);

    QCOMPARE(stateSpyGranted2.count(), 1);
    QCOMPARE(stateSpyLost2.count(), 0);
    QCOMPARE(stateSpyReleased2.count(), 1);
    QCOMPARE(stateSpyDenied2.count(), 0);
    QCOMPARE(stateSpyUpdateOK2.count(), 0);
    QCOMPARE(stateSpyResourcesReleased2.count(), 0);
    QCOMPARE(stateSpyBecameAvailable2.count(), 1);
}

// This test tests simple acquire with two clients
void TestReleasedByManager::testLostBothPlayer()
{
    ResourceSet resourceSet("player");
    ResourceSet resourceSet2("player");

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
    // Wait for released-by-manager -signal for first client
    waitForSignal(&resourceSet, SIGNAL(resourcesReleasedByManager()));
    QCOMPARE(stateSpyResourcesReleased.count(), 1);
    // Wait for the granted-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted2.count(), 1);
    // No Lost-signal
    QCOMPARE(stateSpyLost.count(), 0);

    // Release the resource from the second client
    bool releaseOk2 = resourceSet2.release();
    QVERIFY(releaseOk2);
    // Wait for the released-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased2.count(), 1);

    // Check all the signals
    QCOMPARE(stateSpyGranted.count(), 1);
    QCOMPARE(stateSpyLost.count(), 0);
    QCOMPARE(stateSpyReleased.count(), 0);
    QCOMPARE(stateSpyDenied.count(), 0);
    QCOMPARE(stateSpyUpdateOK.count(), 0);
    QCOMPARE(stateSpyResourcesReleased.count(), 1);
    QCOMPARE(stateSpyBecameAvailable.count(), 1);

    QCOMPARE(stateSpyGranted2.count(), 1);
    QCOMPARE(stateSpyLost2.count(), 0);
    QCOMPARE(stateSpyReleased2.count(), 1);
    QCOMPARE(stateSpyDenied2.count(), 0);
    QCOMPARE(stateSpyUpdateOK2.count(), 0);
    QCOMPARE(stateSpyResourcesReleased2.count(), 0);
    QCOMPARE(stateSpyBecameAvailable2.count(), 1);
}

// This test tests simple acquire with two clients
void TestReleasedByManager::testLostAlwaysReplyNoPlayer()
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
    bool acquireOk = resourceSet.acquire();
    QVERIFY(acquireOk);
    // Wait for the granted-signal for the first client
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted.count(), 1);

    // Acquire the resource for the second client
    bool acquireOk2 = resourceSet2.acquire();
    QVERIFY(acquireOk2);
    // Wait for the lost-signal for the first client
    // The lost-signal for the first client must become before the granted-
    // signal for the second client
    waitForSignal(&resourceSet, SIGNAL(lostResources()));
    //QCOMPARE(stateSpyGranted2.count(), 0);
    QCOMPARE(stateSpyLost.count(), 1);
    // Wait for the granted-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted2.count(), 1);

    // Release the resource from the second client
    bool releaseOk2 = resourceSet2.release();
    QVERIFY(releaseOk2);
    // Wait for the released-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased2.count(), 1);
    // Wait for the granted-signal for the first client
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted.count(), 2);

    // Release the resource from the first client
    bool releaseOk = resourceSet.release();
    QVERIFY(releaseOk);
    // Wait for the released-signal
    waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased.count(), 1);

    // Check all the signals
    QCOMPARE(stateSpyGranted.count(), 2);
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

// This test tests simple acquire with two clients
void TestReleasedByManager::testLostAlwaysReplyFirstPlayer()
{
    ResourceSet resourceSet("player", this, true, false);
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
    bool acquireOk = resourceSet.acquire();
    QVERIFY(acquireOk);
    // Wait for the granted-signal for the first client
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted.count(), 1);

    // Acquire the resource for the second client
    bool acquireOk2 = resourceSet2.acquire();
    QVERIFY(acquireOk2);
    // Wait for the lost-signal for the first client
    // The lost-signal for the first client must become before the granted-
    // signal for the second client
    waitForSignal(&resourceSet, SIGNAL(lostResources()));
    //QCOMPARE(stateSpyGranted2.count(), 0);
    QCOMPARE(stateSpyLost.count(), 1);
    // Wait for the granted-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted2.count(), 1);

    // Release the resource from the second client
    bool releaseOk2 = resourceSet2.release();
    QVERIFY(releaseOk2);
    // Wait for the released-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased2.count(), 1);
    // Wait for the granted-signal for the first client
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted.count(), 2);

    // Release the resource from the first client
    bool releaseOk = resourceSet.release();
    QVERIFY(releaseOk);
    // Wait for the released-signal
    waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased.count(), 1);

    // Check all the signals
    QCOMPARE(stateSpyGranted.count(), 2);
    QCOMPARE(stateSpyLost.count(), 1);
    QCOMPARE(stateSpyReleased.count(), 1);
    QCOMPARE(stateSpyDenied.count(), 0);
    QCOMPARE(stateSpyUpdateOK.count(), 0);
    QCOMPARE(stateSpyResourcesReleased.count(), 0);
    QCOMPARE(stateSpyBecameAvailable.count(), 3);

    QCOMPARE(stateSpyGranted2.count(), 1);
    QCOMPARE(stateSpyLost2.count(), 0);
    QCOMPARE(stateSpyReleased2.count(), 1);
    QCOMPARE(stateSpyDenied2.count(), 0);
    QCOMPARE(stateSpyUpdateOK2.count(), 0);
    QCOMPARE(stateSpyResourcesReleased2.count(), 0);
    QCOMPARE(stateSpyBecameAvailable2.count(), 1);
}

// This test tests simple acquire with two clients
void TestReleasedByManager::testLostAlwaysReplySecondPlayer()
{
    ResourceSet resourceSet("background", this, true, false);
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
    // Wait for the lost-signal for the first client
    // The lost-signal for the first client must become before the granted-
    // signal for the second client
    waitForSignal(&resourceSet, SIGNAL(lostResources()));
    //QCOMPARE(stateSpyGranted2.count(), 0);
    QCOMPARE(stateSpyLost.count(), 1);
    // Wait for the granted-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted2.count(), 1);

    // Release the resource from the second client
    bool releaseOk2 = resourceSet2.release();
    QVERIFY(releaseOk2);
    // Wait for the released-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased2.count(), 1);
    // Wait for the granted-signal for the first client
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted.count(), 2);

    // Release the resource from the first client
    bool releaseOk = resourceSet.release();
    QVERIFY(releaseOk);
    // Wait for the released-signal
    waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased.count(), 1);

    // Check all the signals
    QCOMPARE(stateSpyGranted.count(), 2);
    QCOMPARE(stateSpyLost.count(), 1);
    QCOMPARE(stateSpyReleased.count(), 1);
    QCOMPARE(stateSpyDenied.count(), 0);
    QCOMPARE(stateSpyUpdateOK.count(), 0);
    QCOMPARE(stateSpyResourcesReleased.count(), 0);
    QCOMPARE(stateSpyBecameAvailable.count(), 3);

    QCOMPARE(stateSpyGranted2.count(), 1);
    QCOMPARE(stateSpyLost2.count(), 0);
    QCOMPARE(stateSpyReleased2.count(), 1);
    QCOMPARE(stateSpyDenied2.count(), 0);
    QCOMPARE(stateSpyUpdateOK2.count(), 0);
    QCOMPARE(stateSpyResourcesReleased2.count(), 0);
    QCOMPARE(stateSpyBecameAvailable2.count(), 1);
}

// This test tests simple acquire with two clients
void TestReleasedByManager::testLostAlwaysReplyBothPlayer()
{
    ResourceSet resourceSet("player", this, true, false);
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
    // Wait for released-by-manager -signal for first client
    waitForSignal(&resourceSet, SIGNAL(resourcesReleasedByManager()));
    QCOMPARE(stateSpyResourcesReleased.count(), 1);
    // Wait for the granted-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted2.count(), 1);
    // No Lost-signal
    QCOMPARE(stateSpyLost.count(), 0);

    // Release the resource from the second client
    bool releaseOk2 = resourceSet2.release();
    QVERIFY(releaseOk2);
    // Wait for the released-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased2.count(), 1);

    // Check all the signals
    QCOMPARE(stateSpyGranted.count(), 1);
    QCOMPARE(stateSpyLost.count(), 0);
    QCOMPARE(stateSpyReleased.count(), 1);
    QCOMPARE(stateSpyDenied.count(), 0);
    QCOMPARE(stateSpyUpdateOK.count(), 0);
    QCOMPARE(stateSpyResourcesReleased.count(), 1);
    QCOMPARE(stateSpyBecameAvailable.count(), 1);

    QCOMPARE(stateSpyGranted2.count(), 1);
    QCOMPARE(stateSpyLost2.count(), 0);
    QCOMPARE(stateSpyReleased2.count(), 1);
    QCOMPARE(stateSpyDenied2.count(), 0);
    QCOMPARE(stateSpyUpdateOK2.count(), 0);
    QCOMPARE(stateSpyResourcesReleased2.count(), 0);
    QCOMPARE(stateSpyBecameAvailable2.count(), 1);
}

// This test tests simple acquire with two clients
void TestReleasedByManager::testLostAutoReleaseNoPlayer()
{
    ResourceSet resourceSet("alarm", this, false, true);
    ResourceSet resourceSet2("alarm", this, false, true);

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
    // Wait for the lost-signal for the first client
    // The lost-signal for the first client must become before the granted-
    // signal for the second client
    waitForSignal(&resourceSet, SIGNAL(lostResources()));
    //QCOMPARE(stateSpyGranted2.count(), 0);
    QCOMPARE(stateSpyLost.count(), 1);
    // Wait for the granted-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted2.count(), 1);

    // Release the resource from the second client
    bool releaseOk2 = resourceSet2.release();
    QVERIFY(releaseOk2);
    // Wait for the released-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased2.count(), 1);
    // Wait for the granted-signal for the first client
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted.count(), 2);

    // Release the resource from the first client
    bool releaseOk = resourceSet.release();
    QVERIFY(releaseOk);
    // Wait for the released-signal
    waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased.count(), 1);

    // Check all the signals
    QCOMPARE(stateSpyGranted.count(), 2);
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

// This test tests simple acquire with two clients
void TestReleasedByManager::testLostAutoReleaseFirstPlayer()
{
    ResourceSet resourceSet("player", this, false, true);
    ResourceSet resourceSet2("alarm", this, false, true);

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
    // Wait for the lost-signal for the first client
    // The lost-signal for the first client must become before the granted-
    // signal for the second client
    waitForSignal(&resourceSet, SIGNAL(lostResources()));
    //QCOMPARE(stateSpyGranted2.count(), 0);
    QCOMPARE(stateSpyLost.count(), 1);
    // Wait for the granted-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted2.count(), 1);

    // Release the resource from the second client
    bool releaseOk2 = resourceSet2.release();
    QVERIFY(releaseOk2);
    // Wait for the released-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased2.count(), 1);

    // Check all the signals
    QCOMPARE(stateSpyGranted.count(), 1);
    QCOMPARE(stateSpyLost.count(), 1);
    QCOMPARE(stateSpyReleased.count(), 0);
    QCOMPARE(stateSpyDenied.count(), 0);
    QCOMPARE(stateSpyUpdateOK.count(), 0);
    QCOMPARE(stateSpyResourcesReleased.count(), 0);
    QCOMPARE(stateSpyBecameAvailable.count(), 3);

    QCOMPARE(stateSpyGranted2.count(), 1);
    QCOMPARE(stateSpyLost2.count(), 0);
    QCOMPARE(stateSpyReleased2.count(), 1);
    QCOMPARE(stateSpyDenied2.count(), 0);
    QCOMPARE(stateSpyUpdateOK2.count(), 0);
    QCOMPARE(stateSpyResourcesReleased2.count(), 0);
    QCOMPARE(stateSpyBecameAvailable2.count(), 1);
}

// This test tests simple acquire with two clients
void TestReleasedByManager::testLostAutoReleaseSecondPlayer()
{
    ResourceSet resourceSet("background", this, false, true);
    ResourceSet resourceSet2("player", this, false, true);

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
    // Wait for the lost-signal for the first client
    // The lost-signal for the first client must become before the granted-
    // signal for the second client
    waitForSignal(&resourceSet, SIGNAL(lostResources()));
    //QCOMPARE(stateSpyGranted2.count(), 0);
    QCOMPARE(stateSpyLost.count(), 1);
    // Wait for the granted-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted2.count(), 1);

    // Release the resource from the second client
    bool releaseOk2 = resourceSet2.release();
    QVERIFY(releaseOk2);
    // Wait for the released-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased2.count(), 1);

    // Check all the signals
    QCOMPARE(stateSpyGranted.count(), 1);
    QCOMPARE(stateSpyLost.count(), 1);
    QCOMPARE(stateSpyReleased.count(), 0);
    QCOMPARE(stateSpyDenied.count(), 0);
    QCOMPARE(stateSpyUpdateOK.count(), 0);
    QCOMPARE(stateSpyResourcesReleased.count(), 0);
    QCOMPARE(stateSpyBecameAvailable.count(), 3);

    QCOMPARE(stateSpyGranted2.count(), 1);
    QCOMPARE(stateSpyLost2.count(), 0);
    QCOMPARE(stateSpyReleased2.count(), 1);
    QCOMPARE(stateSpyDenied2.count(), 0);
    QCOMPARE(stateSpyUpdateOK2.count(), 0);
    QCOMPARE(stateSpyResourcesReleased2.count(), 0);
    QCOMPARE(stateSpyBecameAvailable2.count(), 1);
}

// This test tests simple acquire with two clients
void TestReleasedByManager::testLostAutoReleaseBothPlayer()
{
    ResourceSet resourceSet("player", this, false, true);
    ResourceSet resourceSet2("player", this, false, true);

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
    // Wait for the lost-signal for the first client
    // The lost-signal for the first client must become before the granted-
    // signal for the second client
    waitForSignal(&resourceSet, SIGNAL(lostResources()));
    //QCOMPARE(stateSpyGranted2.count(), 0);
    QCOMPARE(stateSpyLost.count(), 1);
    // Wait for the granted-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted2.count(), 1);

    // Release the resource from the second client
    bool releaseOk2 = resourceSet2.release();
    QVERIFY(releaseOk2);
    // Wait for the released-signal for the second client
    waitForSignal(&resourceSet2, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased2.count(), 1);
    // Wait for the granted-signal for the first client
    waitForSignal(&resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)));
    QCOMPARE(stateSpyGranted.count(), 2);

    // Release the resource from the first client
    bool releaseOk = resourceSet.release();
    QVERIFY(releaseOk);
    // Wait for the released-signal
    waitForSignal(&resourceSet, SIGNAL(resourcesReleased()));
    QCOMPARE(stateSpyReleased.count(), 1);

    // Check all the signals
    QCOMPARE(stateSpyGranted.count(), 2);
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


QTEST_MAIN(TestReleasedByManager)
