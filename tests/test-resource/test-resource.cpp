#include "test-resource.h"
#include <QSet>

using namespace ResourcePolicy;

Resource * TestResource::resourceFromType(ResourceType type)
{
    switch(type) {
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
    default:
        return NULL;
    }
}

const char * TestResource::stringFromType(ResourceType type)
{
    switch(type) {
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
}

void TestResource::testType()
{
    for(quint32 type=AudioPlaybackType; type < NumberOfTypes; type++) {
        ResourceType expected = (ResourceType)type;
        Resource *resource = resourceFromType(expected);
        if(resource->type() != expected) {
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
    QTest::newRow("Resource is not optional") << VibraType<< false << false;
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

void TestResource::testShared_data()
{
    QTest::addColumn<ResourceType>("type");
    QTest::addColumn<bool>("shared");
    QTest::addColumn<bool>("expected");
    
    QTest::newRow("Resource is shared") << AudioPlaybackType << true << true;
    QTest::newRow("Resource is not shared") << AudioPlaybackType << false << false;
    QTest::newRow("Resource is shared") << AudioRecorderType << true << true;
    QTest::newRow("Resource is not shared") << AudioRecorderType << false << false;
    QTest::newRow("Resource is shared") << VideoPlaybackType << true << true;
    QTest::newRow("Resource is not shared") << VideoPlaybackType << false << false;
    QTest::newRow("Resource is shared") << VideoRecorderType << true << true;
    QTest::newRow("Resource is not shared") << VideoRecorderType << false << false;
    QTest::newRow("Resource is shared") << VibraType << true << true;
    QTest::newRow("Resource is not shared") << VibraType<< false << false;
    QTest::newRow("Resource is shared") << BacklightType << true << true;
    QTest::newRow("Resource is not shared") << BacklightType << false << false;
    QTest::newRow("Resource is shared") << SystemButtonType << true << true;
    QTest::newRow("Resource is not shared") << SystemButtonType << false << false;
    QTest::newRow("Resource is shared") << LedsType << true << true;
    QTest::newRow("Resource is not shared") << LedsType << false << false;
    QTest::newRow("Resource is shared") << LockButtonType << true << true;
    QTest::newRow("Resource is not shared") << LockButtonType << false << false;
    QTest::newRow("Resource is shared") << ScaleButtonType << true << true;
    QTest::newRow("Resource is not shared") << ScaleButtonType << false << false;
    QTest::newRow("Resource is shared") << SnapButtonType << true << true;
    QTest::newRow("Resource is not shared") << SnapButtonType << false << false;
    QTest::newRow("Resource is shared") << LensCoverType << true << true;
    QTest::newRow("Resource is not shared") << LensCoverType << false << false;
}

void TestResource::testShared()
{
    QFETCH(ResourceType, type);
    QFETCH(bool, shared);
    QFETCH(bool, expected);

    Resource *resource = resourceFromType(type);
    resource->setShared(shared);

    bool result = resource->isShared();

    QCOMPARE(result, expected);
}

void TestResource::testGranted_data()
{
    QTest::addColumn<ResourceType>("type");
    QTest::addColumn<bool>("granted");
    QTest::addColumn<bool>("expected");
    
    QTest::newRow("Resource is granted") << AudioPlaybackType << true << true;
    QTest::newRow("Resource is denied") << AudioPlaybackType << false << false;
    QTest::newRow("Resource is granted") << AudioRecorderType << true << true;
    QTest::newRow("Resource is denied") << AudioRecorderType << false << false;
    QTest::newRow("Resource is granted") << VideoPlaybackType << true << true;
    QTest::newRow("Resource is denied") << VideoPlaybackType << false << false;
    QTest::newRow("Resource is granted") << VideoRecorderType << true << true;
    QTest::newRow("Resource is denied") << VideoRecorderType << false << false;
    QTest::newRow("Resource is granted") << VibraType << true << true;
    QTest::newRow("Resource is denied") << VibraType<< false << false;
    QTest::newRow("Resource is granted") << BacklightType << true << true;
    QTest::newRow("Resource is denied") << BacklightType << false << false;
    QTest::newRow("Resource is granted") << SystemButtonType << true << true;
    QTest::newRow("Resource is denied") << SystemButtonType << false << false;
    QTest::newRow("Resource is granted") << LedsType << true << true;
    QTest::newRow("Resource is denied") << LedsType << false << false;
    QTest::newRow("Resource is granted") << LockButtonType << true << true;
    QTest::newRow("Resource is denied") << LockButtonType << false << false;
    QTest::newRow("Resource is granted") << ScaleButtonType << true << true;
    QTest::newRow("Resource is denied") << ScaleButtonType << false << false;
    QTest::newRow("Resource is granted") << SnapButtonType << true << true;
    QTest::newRow("Resource is denied") << SnapButtonType << false << false;
    QTest::newRow("Resource is granted") << LensCoverType << true << true;
    QTest::newRow("Resource is denied") << LensCoverType << false << false;
}

void TestResource::testGranted()
{
    QFETCH(ResourceType, type);
    QFETCH(bool, granted);
    QFETCH(bool, expected);

    Resource *resource = resourceFromType(type);

    if(granted)
        resource->setGranted();
    else
        resource->unsetGranted();
    bool result = resource->isGranted();

    QVERIFY(result == expected);
}

void TestResource::testClone()
{
    for(quint32 type=AudioPlaybackType; type < NumberOfTypes; type++) {
        Resource *resource = resourceFromType((ResourceType)type);
        resource->setOptional();
        Resource *copy = resource->clone();

        QCOMPARE(copy->isOptional(), resource->isOptional());
        QCOMPARE(copy->isShared(), resource->isShared());
        QCOMPARE(copy->isGranted(), resource->isGranted());
        QCOMPARE(copy->type(), resource->type());
        delete copy;
    }
}

QTEST_MAIN(TestResource)

