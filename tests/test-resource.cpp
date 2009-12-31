#include "test-resource.h"
TestResource::TestResource()
{
}

TestResource::~TestResource()
{
}

void TestResource::initTestCase()
{
    resource = new Resource(MediaClass, RP_FLAGS_AUDIO|RP_FLAGS_VIDEO, this);
    resourceLibrary = new MockResourceLibrary(resource, false);
    QVERIFY(resource != NULL);
    QVERIFY(resource->resourceClass == MediaClass);
    QVERIFY(resource->flags == (RP_FLAGS_AUDIO|RP_FLAGS_VIDEO));
}

void TestResource::testInitializeSucceeds()
{
    bool initializeSucceeded = resource->initialize(resourceLibrary);

    QVERIFY(resource->resourceLibrary == resourceLibrary);
    QVERIFY(initializeSucceeded);
}

void TestResource::testInitializeFails()
{
    MockResourceLibrary *mockResourceLibrary =
	static_cast<MockResourceLibrary *>(resourceLibrary);
    mockResourceLibrary->makeInitializeFail();

    bool initializeSucceeded = resource->initialize(resourceLibrary);

    QEXPECT_FAIL("", "Expecting resourceLibrary->initialize() to fail", Continue);
    QVERIFY(initializeSucceeded);
    delete resource;
}

QTEST_MAIN(TestResource)
