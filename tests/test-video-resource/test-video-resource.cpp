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
#include <policy/resource.h>
#include <policy/resources.h>
#include "test-video-resource.h"

using namespace ResourcePolicy;

TestVideoResource::TestVideoResource()
    : videoResource(NULL)
{
}

TestVideoResource::~TestVideoResource()
{
}

void TestVideoResource::testConstruct1()
{
    VideoResource *videoResource = new VideoResource;
    QVERIFY(videoResource);

    // Test that no signals get emitted
    QSignalSpy stateSpy(videoResource,
            SIGNAL(videoPropertiesChanged(quint32)));
    QVERIFY(stateSpy.isValid());

    QCOMPARE(videoResource->processID(), (quint32) 0);

    QCOMPARE(stateSpy.count(), 0);
    delete videoResource;
}

void TestVideoResource::testConstruct2()
{
    VideoResource *videoResource = new VideoResource(15);
    QVERIFY(videoResource);

    // Test that no signals get emitted
    QSignalSpy stateSpy(videoResource, SIGNAL(videoPropertiesChanged(quint32)));
    QVERIFY(stateSpy.isValid());

    QCOMPARE(videoResource->processID(), (quint32) 15);

    QCOMPARE(stateSpy.count(), 0);
    delete videoResource;
}

void TestVideoResource::testSetProcessId()
{
    VideoResource *videoResource = new VideoResource();
    QVERIFY(videoResource);

    // Test that signal gets emitted
    QSignalSpy stateSpy(videoResource, SIGNAL(videoPropertiesChanged(quint32)));
    QVERIFY(stateSpy.isValid());

    videoResource->setProcessID(2345);
    QCOMPARE(videoResource->processID(), (quint32) 2345);

    // Check the signal parameters
    QCOMPARE(stateSpy.count(), 1);
    QList<QVariant> signalArgs = stateSpy.takeFirst();
    quint32 signalPid = signalArgs.at(0).toUInt();
    QCOMPARE(signalPid, (quint32) 2345);

    delete videoResource;
}

QTEST_MAIN(TestVideoResource)
