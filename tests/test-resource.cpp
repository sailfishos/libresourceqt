#include "test-resource.h"

using namespace ResourceTypes;

TestResource::TestResource()
    : isReservable(false), isReserved(false)
{
}

TestResource::~TestResource()
{
}

void TestResource::init()
{
    resource = new Resource(MediaClass, AudioResource|VideoResource, this);
    resourceLibrary = new MockResourceLibrary(resource);
    QVERIFY(resource != NULL);
    QVERIFY(resource->applicationClass() == MediaClass);
    QVERIFY(resource->resources() == (AudioResource|VideoResource));
    QVERIFY(resource->isReserved() == false);
    QVERIFY(resource->hasResource(AudioResource));
    QVERIFY(resource->hasResource(VideoResource));
    isReservable =false;
    isReserved = false;
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

// testStateChanged

void TestResource::testReservable()
{
    resource->initialize(resourceLibrary);
    resource->connectToServer();

    QObject::connect(resource, SIGNAL(reservable()), this, SLOT(handleReservable()));

    resource->emitReservable();

    QVERIFY(isReservable);
}

void TestResource::handleReservable()
{
    isReservable = true;
}

void TestResource::testReserve()
{
    resource->initialize(resourceLibrary);
    QObject::connect(resource, SIGNAL(stateChanged(enum ResourceState)),
		     this, SLOT(handleStateChanged(enum ResourceState)));
    resource->connectToServer();

    bool reserveSucceeds = resource->reserve();

    QVERIFY(resource->isReserved());
    QVERIFY(reserveSucceeds);
    QVERIFY(isReserved);
}

void TestResource::testReserveExpectFail()
{
    MockResourceLibrary *mockResourceLibrary =
	static_cast<MockResourceLibrary *>(resourceLibrary);

    resource->initialize(resourceLibrary);
    QObject::connect(resource, SIGNAL(stateChanged(enum ResourceState)),
		     this, SLOT(handleStateChanged(enum ResourceState)));
    resource->connectToServer();

    mockResourceLibrary->makeReserveFail();
    bool reserveSucceeds = resource->reserve();

    QVERIFY(reserveSucceeds);
    QEXPECT_FAIL("", "Expecting resourceLibrary->reserve() to fail", Continue);
    QVERIFY(isReserved);
}

void TestResource::handleStateChanged(enum ResourceState newState)
{
    switch(newState) {
    case OwnedState:
	isReserved = true;
	break;
    default:
	isReserved = false;
	break;
    }
}

QTEST_MAIN(TestResource)
