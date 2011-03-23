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



#include "test-resource.h"
#include <QSet>
#include <QtDebug>

using namespace ResourcePolicy;

Resource * TestResource::resourceFromType(ResourceType type)
{
    switch (type) {
    case AudioPlaybackType:
        return audioResource;
    case AudioRecorderType:
        return audioRecorderResource;
    case VideoPlaybackType:
        return videoResource;
    case VideoRecorderType:
        return videoRecorderResource;
    case VibraType:
        return vibraResource;
    case LedsType:
        return ledsResource;
    case BacklightType:
        return backlightResource;
    case SystemButtonType:
        return systemButtonResource;
    case LockButtonType:
        return lockButtonResource;
    case ScaleButtonType:
        return scaleButtonResource;
    case SnapButtonType:
        return snapButtonResource;
    case LensCoverType:
        return lensCoverResource;
    case HeadsetButtonsType:
        return headsetButtonsResource;
    default:
        return NULL;
    }
}

const char * TestResource::stringFromType(ResourceType type)
{
    switch (type) {
    case AudioPlaybackType:
        return "AudioPlaybackType";
    case AudioRecorderType:
        return "AudioRecorderType";
    case VideoPlaybackType:
        return "VideoPlaybackType";
    case VideoRecorderType:
        return "VideoRecorderType";
    case VibraType:
        return "VibraType";
    case LedsType:
        return "LedsType";
    case BacklightType:
        return "BacklightType";
    case SystemButtonType:
        return "SystemButtonType";
    case LockButtonType:
        return "LockButtonType";
    case ScaleButtonType:
        return "ScaleButtonType";
    case SnapButtonType:
        return "SnapButtonType";
    case LensCoverType:
        return "LensCoverType";
    case HeadsetButtonsType:
        return "HeadsetButtonsType";
    default:
        qDebug("Unknown Type 0x%02x requested", type);
        return NULL;
    }
}

TestResource::TestResource()
{
}

TestResource::~TestResource()
{
}

void TestResource::init()
{
    audioResource = new AudioResource;
    audioRecorderResource = new AudioRecorderResource;
    videoResource = new VideoResource;
    videoRecorderResource = new VideoRecorderResource;
    vibraResource = new VibraResource;
    ledsResource = new LedsResource;
    backlightResource = new BacklightResource;
    systemButtonResource = new SystemButtonResource;
    lockButtonResource = new LockButtonResource;
    scaleButtonResource = new ScaleButtonResource;
    snapButtonResource = new SnapButtonResource;
    lensCoverResource = new LensCoverResource;
    headsetButtonsResource = new HeadsetButtonsResource;
}

void TestResource::cleanup()
{
    delete audioResource;
    delete audioRecorderResource;
    delete videoResource;
    delete videoRecorderResource;
    delete vibraResource;
    delete ledsResource;
    delete backlightResource;
    delete systemButtonResource;
    delete lockButtonResource;
    delete scaleButtonResource;
    delete snapButtonResource;
    delete lensCoverResource;
    delete headsetButtonsResource;
}

void TestResource::testType()
{
    for (quint32 type = AudioPlaybackType; type < NumberOfTypes; type++) {
        ResourceType expected = (ResourceType)type;
        Resource *resource = resourceFromType(expected);
        if (resource->type() != expected) {
            qDebug("expected ResourceType = %s, got %s",
                   stringFromType(expected), stringFromType(resource->type()));
        }
        QVERIFY(resource->type() == expected);
    }
}

void TestResource::testOptional_data()
{
    QTest::addColumn<ResourceType>("type");
    QTest::addColumn<bool>("optional");
    QTest::addColumn<bool>("expected");

    QTest::newRow("Resource is optional") << AudioPlaybackType << true << true;
    QTest::newRow("Resource is not optional") << AudioPlaybackType << false << false;
    QTest::newRow("Resource is optional") << AudioRecorderType << true << true;
    QTest::newRow("Resource is not optional") << AudioRecorderType << false << false;
    QTest::newRow("Resource is optional") << VideoPlaybackType << true << true;
    QTest::newRow("Resource is not optional") << VideoPlaybackType << false << false;
    QTest::newRow("Resource is optional") << VideoRecorderType << true << true;
    QTest::newRow("Resource is not optional") << VideoRecorderType << false << false;
    QTest::newRow("Resource is optional") << VibraType << true << true;
    QTest::newRow("Resource is not optional") << VibraType << false << false;
    QTest::newRow("Resource is optional") << BacklightType << true << true;
    QTest::newRow("Resource is not optional") << BacklightType << false << false;
    QTest::newRow("Resource is optional") << SystemButtonType << true << true;
    QTest::newRow("Resource is not optional") << SystemButtonType << false << false;
    QTest::newRow("Resource is optional") << LedsType << true << true;
    QTest::newRow("Resource is not optional") << LedsType << false << false;
    QTest::newRow("Resource is optional") << LockButtonType << true << true;
    QTest::newRow("Resource is not optional") << LockButtonType << false << false;
    QTest::newRow("Resource is optional") << ScaleButtonType << true << true;
    QTest::newRow("Resource is not optional") << ScaleButtonType << false << false;
    QTest::newRow("Resource is optional") << SnapButtonType << true << true;
    QTest::newRow("Resource is not optional") << SnapButtonType << false << false;
    QTest::newRow("Resource is optional") << LensCoverType << true << true;
    QTest::newRow("Resource is not optional") << LensCoverType << false << false;
    QTest::newRow("Resource is optional") << HeadsetButtonsType << true << true;
    QTest::newRow("Resource is not optional") << HeadsetButtonsType << false << false;
}

void TestResource::testOptional()
{
    QFETCH(ResourceType, type);
    QFETCH(bool, optional);
    QFETCH(bool, expected);

    Resource *resource = resourceFromType(type);

    resource->setOptional(optional);
    bool result = resource->isOptional();

    QVERIFY(result == expected);
}

QTEST_MAIN(TestResource)
