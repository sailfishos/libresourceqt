#ifndef TEST_RESOURCE_ENGINE_H
#define TEST_RESOURCE_ENGINE_H

#include <QtTest/QTest>
#include <QMetaType>

class TestResourceEngine;

#include "resource-engine.h"

Q_DECLARE_METATYPE(ResourcePolicy::ResourceType)

class TestResourceEngine: public QObject
{
    Q_OBJECT
private:
    ResourcePolicy::ResourceEngine *resourceEngine;
    ResourcePolicy::AudioResource *audioPlayback;
    ResourcePolicy::VideoResource *videoPlayback;
    ResourcePolicy::AudioRecorderResource *audioRecorder;
    ResourcePolicy::VideoRecorderResource *videoRecorder;
    bool libresourceInitialized;
    bool acquireOrDenyWasCalled;

public:
    ResourcePolicy::ResourceSet *resourceSet;

    TestResourceEngine();
    ~TestResourceEngine();
public slots:
    void connectedHandler();
    void handleDeny();
    void handleAcquire(quint32 bitmaskOfResources);

private slots:
    void init();
    void cleanup();

    void testConnect();
    void testDisconnect();

    void testStatusMessage();

    void testAcquire_data();
    void testAcquire();

    void testRelease_data();
    void testRelease();

    void testRegisterAudioProperties_data();
    void testRegisterAudioProperties();

    void testMultipleInstences();
};

#endif

