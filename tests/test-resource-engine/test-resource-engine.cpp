#include "test-resource-engine.h"
#include <dbus/dbus.h>

using namespace ResourcePolicy;
quint32 theID = 0;

void statusCallbackHandler(resset_t *libresourceSet, resmsg_t *message);

static void verify_resproto_init(resproto_role_t role,
                                 resproto_transport_t transport,
                                 resconn_linkup_t callbackFunction,
                                 DBusConnection *dbusConnection);

static void verify_resconn_connect(resconn_t *connection, resmsg_t *message,
                                   resproto_status_t callbackFunction);
static void verify_resconn_disconnect(resset_t *resourceSet, resmsg_t *message,
                                      resproto_status_t callbackFunction);

TestResourceEngine::TestResourceEngine()
    : resourceEngine(NULL), resourceSet(NULL)
{
    audioPlayback = new AudioResource;
    videoPlayback = new VideoResource;
    audioRecorder = new AudioRecorderResource;
    videoRecorder = new VideoRecorderResource;

    videoPlayback->setOptional();
    audioRecorder->setOptional();
    videoRecorder->setOptional();

    resourceSet = new ResourceSet("player", this);
    theID = resourceSet->id();
    resourceSet->addResource(audioPlayback);
    resourceSet->addResource(videoPlayback);
    resourceSet->addResource(audioRecorder);
    resourceSet->addResource(videoRecorder);
}

TestResourceEngine::~TestResourceEngine()
{
    delete audioPlayback;
    delete videoPlayback;
    delete audioRecorder;
    delete videoRecorder;
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

void TestResourceEngine::testDisconnect()
{
    resourceEngine->connect();
    bool disconnectIsSuccessful = resourceEngine->disconnect();
    QVERIFY(disconnectIsSuccessful);
}

void TestResourceEngine::testStatusMessage()
{
    resourceEngine->connect();
    resourceEngine->messageMap.insert(1, RESMSG_REGISTER);
    QObject::connect(resourceEngine, SIGNAL(connectedToManager()), this, SLOT(connectedHandler()));
    resourceEngine->handleStatusMessage(1);
}

void TestResourceEngine::connectedHandler()
{
    QVERIFY(resourceEngine->isConnected());
}

QTEST_MAIN(TestResourceEngine)

////////////////////////////////////////////////////////////////
resconn_t *resourceConnection;
resset_t  *resSet;

resconn_t* resproto_init(resproto_role_t role, resproto_transport_t transport, ...)
{
    resconn_linkup_t callbackFunction;
    DBusConnection *dbusConnection;
    va_list args;

    va_start(args, transport);
    callbackFunction = va_arg(args, resconn_linkup_t);
    dbusConnection = va_arg(args, DBusConnection *);
    va_end(args);

    verify_resproto_init(role, transport, callbackFunction, dbusConnection);

    resourceConnection =(resconn_t *) calloc(1, sizeof(resconn_t));

    return resourceConnection;
}

static void verify_resproto_init(resproto_role_t role,
                                 resproto_transport_t transport,
                                 resconn_linkup_t callbackFunction,
                                 DBusConnection *dbusConnection)
{
    DBusConnection *systemBus;
    systemBus = dbus_bus_get(DBUS_BUS_SYSTEM, NULL);

    QVERIFY(callbackFunction != NULL);
    QVERIFY(dbusConnection == systemBus);
    QVERIFY(role == RESPROTO_ROLE_CLIENT);
    QVERIFY(transport == RESPROTO_TRANSPORT_DBUS);
}

resset_t  *resconn_connect(resconn_t *connection, resmsg_t *message,
                           resproto_status_t callbackFunction)
{
    verify_resconn_connect(connection, message, callbackFunction);

    resSet = (resset_t *) calloc(1, sizeof(resset_t));

    return resSet;
}

static void verify_resconn_connect(resconn_t *connection, resmsg_t *message,
                                   resproto_status_t callbackFunction)
{
    QVERIFY(connection == resourceConnection);
    QVERIFY(message->record.type == RESMSG_REGISTER);
    QVERIFY(message->record.id == theID);
    QVERIFY(message->record.reqno == 1);
    QVERIFY(message->record.rset.all == (RESMSG_AUDIO_PLAYBACK|RESMSG_AUDIO_RECORDING
                                        |RESMSG_VIDEO_PLAYBACK|RESMSG_VIDEO_RECORDING));
    QVERIFY(message->record.rset.opt == (RESMSG_AUDIO_RECORDING|RESMSG_VIDEO_PLAYBACK
                                        |RESMSG_VIDEO_RECORDING));
    QVERIFY(message->record.rset.share == 0);
    QVERIFY(message->record.rset.mask == 0);
    QCOMPARE(message->record.klass, "player");
    QVERIFY(message->record.mode == 0);
    QVERIFY(callbackFunction != NULL);
}

char *resmsg_res_str(uint32_t res, char *buf, int len)
{
    snprintf(buf, len, "0x%04x", res);

    return buf;
}

int resproto_set_handler(union resconn_u *, resmsg_type_t, resproto_handler_t)
{
    return 1;
}

int resconn_disconnect(resset_t *resSet, resmsg_t *message,
                       resproto_status_t callbackFunction)
{
    verify_resconn_disconnect(resSet, message, callbackFunction);

    return 1;
}

static void verify_resconn_disconnect(resset_t *resourceSet, resmsg_t *message,
                                      resproto_status_t callbackFunction)
{
    qDebug("resourceSet = %p resSet = %p", resourceSet, resSet);
    QVERIFY(resourceSet == resSet);
    QVERIFY(message->record.type == RESMSG_UNREGISTER);
    QVERIFY(message->record.id == theID);
    QVERIFY(message->record.reqno > 1);
    QVERIFY(callbackFunction != NULL);
}

