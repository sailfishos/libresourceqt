#ifndef TEST_RESOURCE_ENGINE_H
#define TEST_RESOURCE_ENGINE_H

#include <QtTest/QTest>
#include "resource-engine.h"

namespace ResourcePolicy
{

class TestResourceEngine: public QObject
{
    Q_OBJECT
private:
    ResourceEngine *resourceEngine;
    AudioResource *audioPlayback;
    VideoResource *videoPlayback;
    AudioRecorderResource *audioRecorder;
    VideoRecorderResource *videoRecorder;
    bool libresourceInitialized;
public:
    ResourceSet *resourceSet;

    TestResourceEngine();
    ~TestResourceEngine();
public slots:
    void connectedHandler();

private slots:
    void init();

    void testConnect();
    void testDisconnect();
    
    void testStatusMessage();
};
}
#endif
