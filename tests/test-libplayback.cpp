#include "test-libplayback.h"

TestLibplayback::TestLibplayback()
{
    resourceFactory=NULL;
    resource=NULL;
}

TestLibplayback::~TestLibplayback()
{
}

void TestLibplayback::initTestCase()
{
    resourceFactory = new ResourceFactory(this);
    QVERIFY(resourceFactory != NULL);
    Resource *resource = resourceFactory->createResource(MediaClass, RP_FLAGS_AUDIO|RP_FLAGS_VIDEO);
    QVERIFY(resource != NULL);
}

void TestLibplayback::testConnectToServer()
{
    resource->connectToServer();
}

QTEST_MAIN(TestLibplayback)
