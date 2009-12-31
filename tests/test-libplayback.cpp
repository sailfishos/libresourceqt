#include "test-libplayback.h"

TestLibplayback::TestLibplayback()
{
}

TestLibplayback::~TestLibplayback()
{
}

void TestLibplayback::initTestCase()
{
    resourceFactory = new ResourceFactory(this);
    QVERIFY(resourceFactory != NULL);
}

void TestLibplayback::testCreateResource()
{
    Resource *resource = resourceFactory->createResource(MediaClass, RP_FLAGS_AUDIO|RP_FLAGS_VIDEO);
    QVERIFY(resource != NULL);
}

QTEST_MAIN(TestLibplayback)
