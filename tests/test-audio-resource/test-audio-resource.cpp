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
#include "test-audio-resource.h"

using namespace ResourcePolicy;

TestAudioResource::TestAudioResource()
    : audioResource(NULL)
{
}

TestAudioResource::~TestAudioResource()
{
}

void TestAudioResource::testConstruct1()
{
    AudioResource *audioResource = new AudioResource;
    QVERIFY(audioResource);

    // Test that no signals get emitted
    QSignalSpy stateSpy(audioResource,
            SIGNAL(audioPropertiesChanged(const QString&, quint32,
            const QString&, const QString &)));
    QVERIFY(stateSpy.isValid());

    QCOMPARE(audioResource->audioGroupIsSet(), false);
    QCOMPARE(audioResource->audioGroup(), QString(""));
    QCOMPARE(audioResource->processID(), (quint32) 0);
    QCOMPARE(audioResource->streamTagIsSet(), false);
    QCOMPARE(audioResource->streamTagName(), QString(""));
    QCOMPARE(audioResource->streamTagValue(), QString(""));

    QCOMPARE(stateSpy.count(), 0);
    delete audioResource;
}

void TestAudioResource::testConstruct2()
{
    AudioResource *audioResource = new AudioResource("testing");
    QVERIFY(audioResource);

    // Test that no signals get emitted
    QSignalSpy stateSpy(audioResource,
            SIGNAL(audioPropertiesChanged(const QString&, quint32,
            const QString&, const QString &)));
    QVERIFY(stateSpy.isValid());

    QCOMPARE(audioResource->audioGroupIsSet(), true);
    QCOMPARE(audioResource->audioGroup(), QString("testing"));
    QCOMPARE(audioResource->processID(), (quint32) 0);
    QCOMPARE(audioResource->streamTagIsSet(), false);
    QCOMPARE(audioResource->streamTagName(), QString(""));
    QCOMPARE(audioResource->streamTagValue(), QString(""));

    QCOMPARE(stateSpy.count(), 0);
    delete audioResource;
}

void TestAudioResource::testSetAudioGroup()
{
    AudioResource *audioResource = new AudioResource();
    QVERIFY(audioResource);

    // Test that signal gets emitted
    QSignalSpy stateSpy(audioResource,
            SIGNAL(audioPropertiesChanged(const QString&, quint32,
            const QString&, const QString &)));
    QVERIFY(stateSpy.isValid());

    audioResource->setAudioGroup("foobar");
    QCOMPARE(audioResource->audioGroupIsSet(), true);
    QCOMPARE(audioResource->audioGroup(), QString("foobar"));
    QCOMPARE(audioResource->processID(), (quint32) 0);
    QCOMPARE(audioResource->streamTagIsSet(), false);
    QCOMPARE(audioResource->streamTagName(), QString(""));
    QCOMPARE(audioResource->streamTagValue(), QString(""));

    // Check the signal parameters
    QCOMPARE(stateSpy.count(), 1);
    QList<QVariant> signalArgs = stateSpy.takeFirst();
    QString signalGroup = signalArgs.at(0).toString();
    QCOMPARE(signalGroup, QString("foobar"));
    quint32 signalPid = signalArgs.at(1).toUInt();
    QCOMPARE(signalPid, (quint32) 0);
    QString signalName = signalArgs.at(2).toString();
    QCOMPARE(signalName, QString(""));
    QString signalValue = signalArgs.at(3).toString();
    QCOMPARE(signalValue, QString(""));

    delete audioResource;
}

void TestAudioResource::testSetProcessId()
{
    AudioResource *audioResource = new AudioResource();
    QVERIFY(audioResource);

    // Test that signal gets emitted
    QSignalSpy stateSpy(audioResource,
            SIGNAL(audioPropertiesChanged(const QString&, quint32,
            const QString&, const QString &)));
    QVERIFY(stateSpy.isValid());

    audioResource->setProcessID(2345);
    QCOMPARE(audioResource->audioGroupIsSet(), false);
    QCOMPARE(audioResource->audioGroup(), QString(""));
    QCOMPARE(audioResource->processID(), (quint32) 2345);
    QCOMPARE(audioResource->streamTagIsSet(), false);
    QCOMPARE(audioResource->streamTagName(), QString(""));
    QCOMPARE(audioResource->streamTagValue(), QString(""));

    // Check the signal parameters
    QCOMPARE(stateSpy.count(), 1);
    QList<QVariant> signalArgs = stateSpy.takeFirst();
    QString signalGroup = signalArgs.at(0).toString();
    QCOMPARE(signalGroup, QString(""));
    quint32 signalPid = signalArgs.at(1).toUInt();
    QCOMPARE(signalPid, (quint32) 2345);
    QString signalName = signalArgs.at(2).toString();
    QCOMPARE(signalName, QString(""));
    QString signalValue = signalArgs.at(3).toString();
    QCOMPARE(signalValue, QString(""));

    delete audioResource;
}

void TestAudioResource::testSetStreamTag()
{
    AudioResource *audioResource = new AudioResource;
    QVERIFY(audioResource);

    // Test that signal gets emitted
    QSignalSpy stateSpy(audioResource,
            SIGNAL(audioPropertiesChanged(const QString&, quint32,
            const QString&, const QString &)));
    QVERIFY(stateSpy.isValid());

    audioResource->setStreamTag("tagname", "tagvalue");
    QCOMPARE(audioResource->audioGroupIsSet(), false);
    QCOMPARE(audioResource->audioGroup(), QString(""));
    QCOMPARE(audioResource->processID(), (quint32) 0);
    QCOMPARE(audioResource->streamTagIsSet(), true);
    QCOMPARE(audioResource->streamTagName(), QString("tagname"));
    QCOMPARE(audioResource->streamTagValue(), QString("tagvalue"));

    // Check the signal parameters
    QCOMPARE(stateSpy.count(), 1);
    QList<QVariant> signalArgs = stateSpy.takeFirst();
    QString signalGroup = signalArgs.at(0).toString();
    QCOMPARE(signalGroup, QString(""));
    quint32 signalPid = signalArgs.at(1).toUInt();
    QCOMPARE(signalPid, (quint32) 0);
    QString signalName = signalArgs.at(2).toString();
    QCOMPARE(signalName, QString("tagname"));
    QString signalValue = signalArgs.at(3).toString();
    QCOMPARE(signalValue, QString("tagvalue"));

    delete audioResource;
}

QTEST_MAIN(TestAudioResource)
