#ifndef TEST_RESOURCE_ENGINE_H
#define TEST_RESOURCE_ENGINE_H

#include <QtTest/QTest>
#include <QMetaType>
#include "resource-engine.h"

Q_DECLARE_METATYPE(ResourcePolicy::ResourceType)

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
        void handleDeny();
        void handleAcquire(QList<ResourceType> optionalResources);

    private slots:
        void init();

        void testConnect();
        void testDisconnect();

        void testStatusMessage();

        void testAcquire_data();
        void testAcquire();
    };
}
#endif

