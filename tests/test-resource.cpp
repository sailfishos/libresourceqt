#include "test-resource.h"

using namespace ResourceTypes;

TestResource::TestResource()
{
}

TestResource::~TestResource()
{
}

void TestResource::init()
{
    resource = new Resource(MediaClass, AudioResource|VideoResource, this);
    resourceLibrary = new MockResourceLibrary(resource, false, false);
    QVERIFY(resource != NULL);
    QVERIFY(resource->applicationClass() == MediaClass);
    QVERIFY(resource->resources() == (AudioResource|VideoResource));
    QVERIFY(resource->hasExclusiveAccess() == false);
    QVERIFY(resource->hasResource(AudioResource));
    QVERIFY(resource->hasResource(VideoResource));
}

void TestResource::testApplicationClass()
{
    enum ResourceClass expected, actual;

    expected = MediaClass;
    actual = resource->applicationClass();

    QVERIFY(actual == expected);
}

void TestResource::testResources()
{
    quint16 expected, actual;

    expected = AudioResource|VideoResource;
    actual = resource->resources();

    QVERIFY(actual == expected);
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
