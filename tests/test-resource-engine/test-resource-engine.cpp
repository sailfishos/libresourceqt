/*************************************************************************
This file is part of libresourceqt

Copyright (C) 2010 Nokia Corporation.

This library is free software; you can redistribute
it and/or modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation
version 2.1 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
USA.
*************************************************************************/



#include "test-resource-engine.h"
#include <dbus/dbus.h>
#include <string.h>

using namespace ResourcePolicy;
quint32 theID = 0;
quint32 resproto_init_calls = 0;

void statusCallbackHandler(resset_t *libresourceSet, resmsg_t *message);
static bool strverify(const char *a, const char *b);

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

    resourceSet = new ResourceSet(APP_CLASS, this);
    theID = resourceSet->id();
    resourceSet->addResourceObject(audioPlayback);
    resourceSet->addResourceObject(videoPlayback);
    resourceSet->addResourceObject(audioRecorder);
    resourceSet->addResourceObject(videoRecorder);
}

TestResourceEngine::~TestResourceEngine()
{
}

void TestResourceEngine::init()
{
    resproto_init_calls=0;
    resourceEngine = new ResourceEngine(resourceSet);
    bool initializeSucceeded = resourceEngine->initialize();
    acquireOrDenyWasCalled = false;
    QVERIFY(!resourceEngine->isConnectedToManager());
    QVERIFY(initializeSucceeded);
    QVERIFY(resourceSet->id() == theID);
}

void TestResourceEngine::cleanup()
{
    delete(resourceEngine);    

    QVERIFY(ResourceEngine::libresourceConnection == NULL);
    QVERIFY(ResourceEngine::libresourceUsers == 0);
    resproto_init_calls=0;
}

void TestResourceEngine::testConnect()
{
    bool connectIsSuccessful = resourceEngine->connectToManager();

    QVERIFY(connectIsSuccessful);
}

void TestResourceEngine::testDisconnect()
{
    resourceEngine->connectToManager();

    bool disconnectIsSuccessful = resourceEngine->disconnectFromManager();

    QVERIFY(disconnectIsSuccessful);
}

void TestResourceEngine::testStatusMessage()
{
    resourceEngine->connectToManager();

    resourceEngine->messageMap.insert(1, RESMSG_REGISTER);
    QObject::connect(resourceEngine, SIGNAL(connectedToManager()), this, SLOT(connectedHandler()));
    resourceEngine->handleStatusMessage(1);
    //verification happens in mock- and callback functions
}

void TestResourceEngine::connectedHandler()
{
    QVERIFY(resourceEngine->isConnectedToManager());
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

bool messageOperationShoulSucceed;
ResourceType optionalType;
bool requestShouldFail;
const char *requestErrorMessage;

void TestResourceEngine::testAcquire()
{
    QFETCH(bool, acquireSucceeds);
    messageOperationShoulSucceed = acquireSucceeds;
    QFETCH(ResourceType, optionalTypeAvailable);
    optionalType = optionalTypeAvailable;
    resourceEngine->connectToManager();
    QFETCH(bool, requestFails);
    requestShouldFail = requestFails;
    QFETCH(QString, errorMessage);

    QByteArray ba = errorMessage.toLatin1();
    requestErrorMessage = ba.data();

    QObject::connect(resourceEngine, SIGNAL(resourcesGranted(quint32)),
                     this, SLOT(handleAcquire(quint32)));
    QObject::connect(resourceEngine, SIGNAL(resourcesDenied()),
                     this, SLOT(handleDeny()));
    bool acquireRequestSucceeded = resourceEngine->acquireResources();

    QVERIFY(acquireRequestSucceeded == !requestShouldFail);
    QVERIFY(acquireOrDenyWasCalled);
}

void TestResourceEngine::handleAcquire(quint32 bitmaskOfGrantedResources)
{
    //qDebug("Acquired resources: 0x%04x", bitmaskOfGrantedResources);
    QVERIFY(messageOperationShoulSucceed);
    bool hasOptionalResource = false;
    QList<Resource *> resourceList = resourceSet->resources();
    for (int i=0; i < resourceList.size(); i++) {
        quint32 bitmask = resourceTypeToLibresourceType(resourceList.at(i)->type());
        //qDebug("Checking if resource %x(%x) is in the list", resourceList.at(i)->type(), bitmask);
        if ((bitmask & bitmaskOfGrantedResources) == bitmask)
        {
            //qDebug("Yes :)");
            if (resourceList.at(i)->type() == optionalType) {
                //qDebug("Resource is the one we are looking for. :D");
                hasOptionalResource = true;
                break;
            }
        }
        else {
            //qDebug("No :(");
        }
    }
    if ((optionalType == NumberOfTypes) && (bitmaskOfGrantedResources > 0)) {
        //qDebug("Expected to get no optional resources");
        hasOptionalResource = true;
    }
    QVERIFY(hasOptionalResource);
    acquireOrDenyWasCalled = true;
}

void TestResourceEngine::handleDeny()
{
    QVERIFY(!messageOperationShoulSucceed);
    acquireOrDenyWasCalled = true;
}

void TestResourceEngine::testRelease_data()
{
    QTest::addColumn<bool>("releaseSucceeds");
    QTest::addColumn<bool>("requestFails");
    QTest::addColumn<QString>("errorMessage");

    QTest::newRow("release succeeds") << true << false << "";
    QTest::newRow("release fails") << false << false << "";
    QTest::newRow("release error, optional VideoPlaybackType") << true << true << "This is a simulated error";
}

void TestResourceEngine::testRelease()
{
    QFETCH(bool, releaseSucceeds);
    messageOperationShoulSucceed = releaseSucceeds;
    resourceEngine->connectToManager();
    QFETCH(bool, requestFails);
    requestShouldFail = requestFails;
    QFETCH(QString, errorMessage);

    QByteArray ba = errorMessage.toLatin1();
    requestErrorMessage = ba.data();

    resourceEngine->connectToManager();

    QObject::connect(resourceEngine, SIGNAL(resourcesGranted(quint32)),
                      this, SLOT(handleAcquire(quint32)));
    QObject::connect(resourceEngine, SIGNAL(resourcesDenied()),
                      this, SLOT(handleDeny()));

    bool releaseRequestSucceeded = resourceEngine->releaseResources();
    QVERIFY(releaseRequestSucceeded == !requestShouldFail);
}

void TestResourceEngine::testRegisterAudioProperties_data()
{
    QTest::addColumn<QString>("audioGroup");
    QTest::addColumn<quint32>("pid");
    QTest::addColumn<QString>("streamName");
    QTest::addColumn<QString>("streamValue");

    QTest::newRow("'player':12345:'media.role':'music'") << "player" << 12345u << "media.role" << "music";
    QTest::newRow("'':12345:'media.role':'music'") << "" << 12345u << "media.role" << "music";
    QTest::newRow(":12345:'media.role':'music'") << QString() << 12345u << "media.role" << "music";
    QTest::newRow("'player':0:'media.role':'music'") << "player" << 0u << "media.role" << "music";
    QTest::newRow("'player':12345:'':'music'") << "player" << 12345u << "" << "music";
    QTest::newRow("'player':12345::'music'") << "player" << 12345u << QString() << "music";
    QTest::newRow("'player':12345:'':''") << "player" << 12345u << "" << "";
    QTest::newRow("'player':12345::") << "player" << 12345u << QString() << QString();
    QTest::newRow("'player':12345:'media.role':''") << "player" << 12345u << "media.role" << "";
    QTest::newRow("'player':12345:'media.role':") << "player" << 12345u << "media.role" << QString();
    QTest::newRow("'':0:'':''") << "" << 0u << "" << "";
    QTest::newRow(":0::") << QString() << 0u << QString() << QString();
}

char *expectedGroup, *expectedStreamName, *expectedStreamValue;

void TestResourceEngine::testRegisterAudioProperties()
{
    QFETCH(QString, audioGroup);
    QFETCH(quint32, pid);
    QFETCH(QString, streamName);
    QFETCH(QString, streamValue);

    requestShouldFail = false;

    if(audioGroup.isEmpty() || audioGroup.isNull()) {
        expectedGroup = APP_CLASS;
    }
    else {
        QByteArray ba = audioGroup.toLatin1();
        expectedGroup = strdup(ba.data());
    }

    if(streamName.isEmpty() || streamName.isNull() || streamValue.isEmpty() || streamValue.isNull()) {
        expectedStreamName = NULL;
        expectedStreamValue = NULL;
    }
    else {
        QByteArray ba = streamName.toLatin1();
        expectedStreamName = strdup(ba.data());
        ba = streamValue.toLatin1();
        expectedStreamValue = strdup(ba.data());
    }
    //qDebug() << "streamValue = " << streamValue;
    //qDebug("expectedStreamValue = %s", expectedStreamValue);

    resourceEngine->connectToManager();

    bool audioPropertiesSetSucceeded = resourceEngine->registerAudioProperties(audioGroup, pid, streamName, streamValue);
    QVERIFY(audioPropertiesSetSucceeded);
}

void TestResourceEngine::testMultipleInstences()
{
    ResourceSet *resSet;
    ResourceEngine *resEngine;
    resconn_t * resConn = ResourceEngine::libresourceConnection;
    resSet = new ResourceSet("background", this);
    resEngine = new ResourceEngine(resSet);
    bool initializeSucceeded = resEngine->initialize();
    acquireOrDenyWasCalled = false;

    QVERIFY(!resEngine->isConnectedToManager());
    QVERIFY(initializeSucceeded);
    QVERIFY(resEngine->id() != theID);
    QVERIFY(ResourceEngine::libresourceConnection == resConn);
    QVERIFY(ResourceEngine::libresourceUsers == 2);

    delete(resEngine);
    QVERIFY(ResourceEngine::libresourceConnection == resConn);
    delete(resSet);
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

    resproto_init_calls += 1;

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
    qDebug("resproto_init_calls==%u", resproto_init_calls);
    QVERIFY(resproto_init_calls==1);
}

resset_t  *resconn_connect(resconn_t *connection, resmsg_t *message,
                           resproto_status_t callbackFunction)
{
    verify_resconn_connect(connection, message, callbackFunction);
    resmsg_t statusMessage;

    resSet = (resset_t *) calloc(1, sizeof(resset_t));
    statusMessage.type = RESMSG_STATUS;
    statusMessage.status.errcod = 0;
    statusMessage.status.errmsg = 0;
    statusMessage.record.id = 11;
    statusMessage.record.reqno = message->record.reqno;
    return resSet;
}

static void verify_resconn_connect(resconn_t *connection, resmsg_t *message,
                                   resproto_status_t callbackFunction)
{
    QVERIFY(connection == resourceConnection);
    QVERIFY(message->record.type == RESMSG_REGISTER);
    QVERIFY(message->record.id == theID);
    QVERIFY(message->record.reqno > 0);
    QVERIFY(message->record.rset.all == (RESMSG_AUDIO_PLAYBACK | RESMSG_AUDIO_RECORDING
                                         | RESMSG_VIDEO_PLAYBACK | RESMSG_VIDEO_RECORDING));
    QVERIFY(message->record.rset.opt == (RESMSG_AUDIO_RECORDING | RESMSG_VIDEO_PLAYBACK
                                         | RESMSG_VIDEO_RECORDING));
    QVERIFY(message->record.rset.share == 0);
    QVERIFY(message->record.rset.mask == 0);
    QCOMPARE(message->record.klass, APP_CLASS);
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
    //qDebug("resourceSet = %p resSet = %p", resourceSet, resSet);
    QVERIFY(resourceSet == resSet);
    QVERIFY(message->record.type == RESMSG_UNREGISTER);
    QVERIFY(message->record.id == theID);
    QVERIFY(message->record.reqno > 1);
    QVERIFY(callbackFunction != NULL);
}

int resproto_send_message(resset_t *resourceSet, resmsg_t *message,
                          resproto_status_t  callbackFunction)
{
    resmsg_t grantMessage, statusMessage;
    //qDebug("%s(): id:%u req#: %u", __FUNCTION__, message->record.id, message->record.reqno);

    verify_resproto_send_message(resourceSet, message, callbackFunction);

    switch (message->record.type) {
    case RESMSG_ACQUIRE:
        statusMessage.type = RESMSG_STATUS;
        statusMessage.status.errcod = requestShouldFail;
        statusMessage.status.errmsg = requestErrorMessage;
        statusMessage.record.id = 11;
        statusMessage.record.reqno = 77;
        callbackFunction(resSet, &statusMessage);

        grantMessage.type = RESMSG_GRANT;
        if (messageOperationShoulSucceed) {
            grantMessage.notify.resrc = RESMSG_AUDIO_PLAYBACK | RESMSG_AUDIO_RECORDING
            | RESMSG_VIDEO_PLAYBACK | RESMSG_VIDEO_RECORDING;
        }
        else {
            grantMessage.notify.resrc = 0;
        }
        grantMessage.record.id = message->record.id;
        grantMessage.record.reqno = message->record.reqno;
        grantCallback(&grantMessage, resSet, resSet->userdata);
            break;
    case RESMSG_RELEASE:
        statusMessage.type = RESMSG_STATUS;
        statusMessage.status.errcod = requestShouldFail;
        statusMessage.status.errmsg = requestErrorMessage;
        statusMessage.record.id = 11;
        statusMessage.record.reqno = 77;
        callbackFunction(resSet, &statusMessage);

        grantMessage.type = RESMSG_GRANT;
        grantMessage.notify.resrc = 0;
        grantMessage.record.id = message->record.id;
        grantMessage.record.reqno = message->record.reqno;
        grantCallback(&grantMessage, resSet, resSet->userdata);
        break;
    case RESMSG_AUDIO:
        statusMessage.type = RESMSG_STATUS;
        statusMessage.status.errcod = 0;
        statusMessage.status.errmsg = "";
        statusMessage.record.id = 11;
        statusMessage.record.reqno = 77;
        callbackFunction(resSet, &statusMessage);
        break;
    default:
        //qDebug("Unknown message requested...");
        return 0;
    }

    if (requestShouldFail)
        return 0;
    else
        return 1;
}

static void verify_resproto_send_message(resset_t *resourceSet, resmsg_t *message,
        resproto_status_t  callbackFunction)
{
    switch (message->record.type) {
    case RESMSG_ACQUIRE:
    case RESMSG_RELEASE:
        QVERIFY(resourceSet == resSet);
        QVERIFY(message->possess.id == theID);
        QVERIFY(callbackFunction != NULL);
        break;
    case RESMSG_AUDIO:
        QVERIFY(resourceSet == resSet);
        QVERIFY(message->possess.id == theID);
        QVERIFY(callbackFunction != NULL);
        QVERIFY(strverify(expectedGroup, message->audio.group));
        QVERIFY(strverify(expectedStreamName, message->audio.property.name));
        QVERIFY(strverify(expectedStreamValue, message->audio.property.match.pattern));
        break;
    default:
        bool expectedMessageType = false;
        QVERIFY(expectedMessageType);
    }
}

static bool strverify(const char *a, const char *b)
{
    //qDebug("a='%s' b='%s'", a, b);
    if (a == b) {
        return true;
    }
    else if ((a == NULL) || (b == NULL)) {
        return false;
    }
    else {
        int i = strcmp(a, b);
        if (i == 0)
            return true;
        else
            return false;
    }

}

char *resmsg_dump_message(resmsg_t *, int, char *, int)
{
    char * ret = strdup("message");
    return ret;
}

