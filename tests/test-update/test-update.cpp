/*************************************************************************
This file is part of libresourceqt

Copyright (C) 2011 Nokia Corporation.

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
#include "test-update.h"

using namespace ResourcePolicy;

TestUpdate::TestUpdate()
{
}

TestUpdate::~TestUpdate()
{
}

void TestUpdate::waitForSignal(const QObject *sender, const char *signal, quint32 timeout)
{
    QEventLoop loop;
    QTimer::singleShot(timeout, &loop, SLOT(quit()));
    loop.connect(sender, signal, SLOT(quit()));
    loop.exec();
}

void TestUpdate::testUpdate()
{
    ResourceSet resourceSet("player");

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

    // Check the released signal was received
    QCOMPARE(stateSpyReleased.count(), 1);
    QCOMPARE(stateSpyUpdateOK.count(), 0);
    QCOMPARE(stateSpyGranted.count(), 1);

    QCOMPARE(stateSpyGranted.count(), 1);
    QCOMPARE(stateSpyLost.count(), 0);
    QCOMPARE(stateSpyReleased.count(), 1);
    QCOMPARE(stateSpyDenied.count(), 0);
    QCOMPARE(stateSpyUpdateOK.count(), 0);
    QCOMPARE(stateSpyResourcesReleased.count(), 0);
    QCOMPARE(stateSpyBecameAvailable.count(), 2);
}

void TestUpdate::testUpdateGranted()
{
    ResourceSet resourceSet("player");

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

    QCOMPARE(stateSpyGranted.count(), 2);
    QCOMPARE(stateSpyLost.count(), 0);
    QCOMPARE(stateSpyReleased.count(), 1);
    QCOMPARE(stateSpyDenied.count(), 0);
    QCOMPARE(stateSpyUpdateOK.count(), 0);
    QCOMPARE(stateSpyResourcesReleased.count(), 0);
    QCOMPARE(stateSpyBecameAvailable.count(), 2);
}

QTEST_MAIN(TestUpdate)
