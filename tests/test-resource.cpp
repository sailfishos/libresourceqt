#include "test-resource.h"
TestResource::TestResource()
{
}

TestResource::~TestResource()
{
}

void TestResource::init()
{
    resource = new Resource(MediaClass, RP_FLAGS_AUDIO|RP_FLAGS_VIDEO, this);
    resourceLibrary = new MockResourceLibrary(resource, false, false);
    QVERIFY(resource != NULL);
    QVERIFY(resource->resourceClass == MediaClass);
    QVERIFY(resource->flags == (RP_FLAGS_AUDIO|RP_FLAGS_VIDEO));
    qDebug("INIT!");
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
}

void TestResource::testConnectToServerSucceeds()
{
    bool initializeSucceeded = resource->initialize(resourceLibrary);

    QVERIFY(resource->resourceLibrary == resourceLibrary);
    QVERIFY(initializeSucceeded);

    bool connectToServerSucceeded = resource->connectToServer();
    QVERIFY(connectToServerSucceeded);
}

void TestResource::testConnectToServerFails()
{
    MockResourceLibrary *mockResourceLibrary =
	static_cast<MockResourceLibrary *>(resourceLibrary);

    bool initializeSucceeded = resource->initialize(resourceLibrary);

    QVERIFY(resource->resourceLibrary == resourceLibrary);
    QVERIFY(initializeSucceeded);

    mockResourceLibrary->makeServerConnectFail();
    bool connectToServerSucceeded = resource->connectToServer();

    QEXPECT_FAIL("", "Expecting resourceLibrary->connectToServer() to fail", Continue);
    QVERIFY(connectToServerSucceeded);
}

QTEST_MAIN(TestResource)
