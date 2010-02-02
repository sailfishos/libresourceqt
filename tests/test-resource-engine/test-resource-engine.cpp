#include "test-resource-engine.h"
#include <dbus/dbus.h>

using namespace ResourcePolicy;

TestResourceEngine::TestResourceEngine()
    : resourceEngine(NULL), resourceSet(NULL),
      audioPlayback(AudioPlaybackResource), videoPlayback(VideoPlaybackResource),
      audioRecorder(AudioRecorderResource), videoRecorder(VideoRecorderResource)
{
    resourceSet = new ResourceSet("player", this);
    resourceSet->addResource(audioPlayback);
    resourceSet->addResource(videoPlayback);
    resourceSet->addResource(audioRecorder);
    resourceSet->addResource(videoRecorder);
}

TestResourceEngine::~TestResourceEngine()
{
}

void TestResourceEngine::init()
{
    resourceEngine = new ResourceEngine(resourceSet);
    bool initializeSucceeded = resourceEngine->initialize();
    QVERIFY(!resourceEngine->isConnected());
    QVERIFY(initializeSucceeded);
}

void TestResourceEngine::testConnect()
{
    bool connectIsSuccessful = resourceEngine->connect();
    QVERIFY(connectIsSuccessful);
}

QTEST_MAIN(TestResourceEngine)

////////////////////////////////////////////////////////////////

resconn_t* resproto_init(resproto_role_t role, resproto_transport_t transport, ...)
{
    resconn_t *resourceConnection;
    resconn_linkup_t callbackFunction;
    DBusConnection *dbusConnection, systemBus;
    va_list args;

    va_start();
    callbackFunction = va_arg(args, resconn_linkup_t);
    dbusConnection = va_arg(args, DBusConnection *);
    va_end();

    systemBus = dbus_bus_get(DBUS_BUS_SYSTEM);

    QVERIFY(callbackFunction != NULL);
    QVERIFY(dbusConnection == systemBus);
    QVERIFY(role == RESPROTO_ROLE_CLIENT);
    QVERIFY(transport == RESPROTO_TRANSPORT_DBUS);

    resourceConnection =(resconn_t *) calloc(1, sizeof(resconn_t));

    return resourceConnection;
}
