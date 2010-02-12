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
static void verify_resproto_send_message(resset_t *resourceSet, resmsg_t *message,
        resproto_status_t  callbackFunction);

TestResourceEngine::TestResourceEngine()
{
    resourceEngine = NULL;
    acquireOrDenyWasCalled = false;
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
    acquireOrDenyWasCalled = false;
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
    //verification happens in mock- and callback functions
}

void TestResourceEngine::connectedHandler()
{
    QVERIFY(resourceEngine->isConnected());
}

void TestResourceEngine::testAcquire_data()
{
    QTest::addColumn<bool>("acquireSucceeds");
    QTest::addColumn<ResourceType>("optionalTypeAvailable");
    QTest::addColumn<bool>("requestFails");
    QTest::addColumn<QString>("errorMessage");

    QTest::newRow("acquire succeeds, optional VideoPlaybackType") << true << VideoPlaybackType << false << "";
    QTest::newRow("acquire succeeds, optional AudioRecorderType") << true << AudioRecorderType << false << "";
    QTest::newRow("acquire succeeds, optional VideoPlaybackType") << true << VideoPlaybackType << false << "";
    QTest::newRow("acquire succeeds, no optional types") << true << NumberOfTypes << false << "";
    QTest::newRow("acquire fails") << false << NumberOfTypes << false << "";
    QTest::newRow("acquire error, optional VideoPlaybackType") << true << VideoPlaybackType << true << "This is a simulated error";
}

bool acquireShouldSucceed;
ResourceType optionalType;
bool requestShouldFail;
const char *requestErrorMessage;

void TestResourceEngine::testAcquire()
{
    QFETCH(bool, acquireSucceeds);
    acquireShouldSucceed = acquireSucceeds;
    QFETCH(ResourceType, optionalTypeAvailable);
    optionalType = optionalTypeAvailable;
    resourceEngine->connect();
    QFETCH(bool, requestFails);
    requestShouldFail = requestFails;
    QFETCH(QString, errorMessage);

    QByteArray ba = errorMessage.toLatin1();
    requestErrorMessage = ba.data();

    QObject::connect(resourceEngine, SIGNAL(resourcesAcquired(quint32)),
                     this, SLOT(handleAcquire(quint32)));
    QObject::connect(resourceEngine, SIGNAL(resourcesDenied()),
                     this, SLOT(handleDeny()));
    bool acquireRequestSucceeded = resourceEngine->acquireResources();

    QVERIFY(acquireRequestSucceeded == !requestShouldFail);
    QVERIFY(acquireOrDenyWasCalled);
}

void TestResourceEngine::handleAcquire(quint32 bitmaskOfGrantedResources)
{
    qDebug("Acquired resources: 0x%04x", bitmaskOfGrantedResources);
    QVERIFY(acquireShouldSucceed);
    bool hasOptionalResource = false;
    QList<Resource *> resourceList = resourceSet->resources();
    for (int i=0; i < resourceList.size(); i++) {
        quint32 bitmask = resourceTypeToLibresourceType(resourceList.at(i)->type());
        qDebug("Checking if resource %x(%x) is in the list", resourceList.at(i)->type(), bitmask);
        if ((bitmask & bitmaskOfGrantedResources) == bitmask)
        {
            qDebug("Yes :)");
            if (resourceList.at(i)->type() == optionalType) {
                qDebug("Resource is the one we are looking for. :D");
                hasOptionalResource = true;
                break;
            }
        }
        else {
            qDebug("No :(");
        }
    }
    if ((optionalType == NumberOfTypes) && (bitmaskOfGrantedResources > 0)) {
        qDebug("Expected to get no optional resources");
        hasOptionalResource = true;
    }
    QVERIFY(hasOptionalResource);
    acquireOrDenyWasCalled = true;
}

void TestResourceEngine::handleDeny()
{
    QVERIFY(!acquireShouldSucceed);
    acquireOrDenyWasCalled = true;
}

QTEST_MAIN(TestResourceEngine)

////////////////////////////////////////////////////////////////
resconn_t *resourceConnection = NULL;
resset_t  *resSet = NULL;
resproto_handler_t grantCallback = NULL;
resproto_status_t statusCallback = NULL;

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

    resourceConnection = (resconn_t *) calloc(1, sizeof(resconn_t));

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
    QVERIFY(message->record.rset.all == (RESMSG_AUDIO_PLAYBACK | RESMSG_AUDIO_RECORDING
                                         | RESMSG_VIDEO_PLAYBACK | RESMSG_VIDEO_RECORDING));
    QVERIFY(message->record.rset.opt == (RESMSG_AUDIO_RECORDING | RESMSG_VIDEO_PLAYBACK
                                         | RESMSG_VIDEO_RECORDING));
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

int resproto_set_handler(union resconn_u *, resmsg_type_t type,
                         resproto_handler_t callbackFunction)
{
    if (type == RESMSG_GRANT) {
        grantCallback = callbackFunction;
    }
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

int resproto_send_message(resset_t *resourceSet, resmsg_t *message,
                          resproto_status_t  callbackFunction)
{
    resmsg_t replyMessage, statusMessage;

    qDebug("%s(): id:%u req#: %u", __FUNCTION__, message->record.id, message->record.reqno);

    replyMessage.type = RESMSG_GRANT;
    if (acquireShouldSucceed) {
        replyMessage.notify.resrc = RESMSG_AUDIO_PLAYBACK | RESMSG_AUDIO_RECORDING 
                                  | RESMSG_VIDEO_PLAYBACK | RESMSG_VIDEO_RECORDING;
    }
    else {
        replyMessage.notify.resrc = 0;
    }
    replyMessage.record.id = message->record.id;
    replyMessage.record.reqno = message->record.reqno;

    verify_resproto_send_message(resourceSet, message, callbackFunction);

    statusMessage.type = RESMSG_STATUS;
    statusMessage.status.errcod = requestShouldFail;
    statusMessage.status.errmsg = requestErrorMessage;
    statusMessage.record.id = 11;
    statusMessage.record.reqno = 77;
    callbackFunction(resSet, &statusMessage);

    grantCallback(&replyMessage, resSet, resSet->userdata);

    if (requestShouldFail)
        return 0;
    else
        return 1;
}

static void verify_resproto_send_message(resset_t *resourceSet, resmsg_t *message,
        resproto_status_t  callbackFunction)
{
    if (message->record.type == RESMSG_ACQUIRE) {
        QVERIFY(resourceSet == resSet);
        QVERIFY(message->record.id == theID);
        QVERIFY(callbackFunction != NULL);
    }
}

