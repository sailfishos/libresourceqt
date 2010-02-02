#ifndef TEST_RESOURCE_ENGINE_H
#define TEST_RESOURCE_ENGINE_H

#include <QtTest/QTest>
#include "resource-engine.h"

using namespace ResourcePolicy;

class TestResourceEngine: public QObject
{
   Q_OBJECT
   friend resconn_t* resproto_init(resproto_role_t, resproto_transport_t, ...);
private:
   ResourceEngine *resourceEngine;
   ResourceSet *resourceSet;
   Resource audioPlayback;
   Resource videoPlayback;
   Resource audioRecorder;
   Resource videoRecorder;
   bool libresourceInitialized;
public:
   TestResourceEngine();
   ~TestResourceEngine();
public slots:

private slots:
   void init();

   void testConnect();
};

#endif
