#ifndef TEST_RESOURCE_H
#define TEST_RESOURCE_H

#include <QtTest/QTest>
#include <QMetaType>
#include <policy/resources.h>

Q_DECLARE_METATYPE(ResourcePolicy::ResourceType)

using namespace ResourcePolicy;

class TestResource: public QObject
{
    Q_OBJECT
private:
    AudioResource *audioResource;
    AudioRecorderResource *audioRecorderResource;
    Resource *videoResource;
    Resource *videoRecorderResource;
    Resource *vibraResource;
    Resource *ledsResource;
    Resource *backlightResource;
    Resource *systemButtonResource;
    Resource *lockButtonResource;
    Resource *scaleButtonResource;
    Resource *snapButtonResource;
    Resource *lensCoverResource;

    Resource * resourceFromType(ResourceType type);
    const char * stringFromType(ResourceType type);
public:
    TestResource();
    ~TestResource();

private slots:

    void init();
    void cleanup();

    void testType();

    void testOptional_data();
    void testOptional();

    void testShared_data();
    void testShared();

    void testGranted_data();
    void testGranted();

    void testClone();
};

#endif
