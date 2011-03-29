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
    QCOMPARE(audioResource->audioGroupIsSet(), false);
    QCOMPARE(audioResource->audioGroup(), QString(""));
    QCOMPARE(audioResource->processID(), (quint32) 0);
    QCOMPARE(audioResource->streamTagIsSet(), false);
    QCOMPARE(audioResource->streamTagName(), QString(""));
    QCOMPARE(audioResource->streamTagValue(), QString(""));
    delete audioResource;
}

void TestAudioResource::testConstruct2()
{
    AudioResource *audioResource = new AudioResource("testing");
    QVERIFY(audioResource);
    QCOMPARE(audioResource->audioGroupIsSet(), true);
    QCOMPARE(audioResource->audioGroup(), QString("testing"));
    QCOMPARE(audioResource->processID(), (quint32) 0);
    QCOMPARE(audioResource->streamTagIsSet(), false);
    QCOMPARE(audioResource->streamTagName(), QString(""));
    QCOMPARE(audioResource->streamTagValue(), QString(""));
    delete audioResource;
}

void TestAudioResource::testSetAudioGroup()
{
    AudioResource *audioResource = new AudioResource();
    QVERIFY(audioResource);
    audioResource->setAudioGroup("foobar");
    QCOMPARE(audioResource->audioGroupIsSet(), true);
    QCOMPARE(audioResource->audioGroup(), QString("foobar"));
    QCOMPARE(audioResource->processID(), (quint32) 0);
    QCOMPARE(audioResource->streamTagIsSet(), false);
    QCOMPARE(audioResource->streamTagName(), QString(""));
    QCOMPARE(audioResource->streamTagValue(), QString(""));
    delete audioResource;
}

void TestAudioResource::testSetProcessId()
{
    AudioResource *audioResource = new AudioResource();
    QVERIFY(audioResource);
    audioResource->setProcessID(2345);
    QCOMPARE(audioResource->audioGroupIsSet(), false);
    QCOMPARE(audioResource->audioGroup(), QString(""));
    QCOMPARE(audioResource->processID(), (quint32) 2345);
    QCOMPARE(audioResource->streamTagIsSet(), false);
    QCOMPARE(audioResource->streamTagName(), QString(""));
    QCOMPARE(audioResource->streamTagValue(), QString(""));
    delete audioResource;
}

void TestAudioResource::testSetStreamTag()
{
    AudioResource *audioResource = new AudioResource;
    QVERIFY(audioResource);
    audioResource->setStreamTag("tagname", "tagvalue");
    QCOMPARE(audioResource->audioGroupIsSet(), false);
    QCOMPARE(audioResource->audioGroup(), QString(""));
    QCOMPARE(audioResource->processID(), (quint32) 0);
    QCOMPARE(audioResource->streamTagIsSet(), true);
    QCOMPARE(audioResource->streamTagName(), QString("tagname"));
    QCOMPARE(audioResource->streamTagValue(), QString("tagvalue"));
    delete audioResource;
}

QTEST_MAIN(TestAudioResource)
