#include "test-libplayback.h"

using namespace ResourceTypes;

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
    Resource *resource = resourceFactory->createResource(MediaClass, AudioResource|VideoResource);
    QVERIFY(resource != NULL);
}

void TestLibplayback::testConnectToServer()
{
    resource->connectToServer();
}

QTEST_MAIN(TestLibplayback)
