/*************************************************************************
This file is part of libresourceqt

Copyright (C) 2011 Nokia Corporation.

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



#include "dbusconnectioneventloop.h"
#include <stdint.h>
#include <QtTest/QtTest>
#include <unistd.h>

class TestDbusQEventLoop: public QObject
{
    Q_OBJECT

private:
    DBusConnection* 			systemBus;
    DBusConnection* 			sessionBus;

    void resetValues() {
        wasInNotifyFnc = 0;
        responseString = 0;
        responseInt    = 0;
        typeError      = false;
        noResponse     = false;
        timerTimeout   = false;
    }

    void processQTEventLoop(DBusPendingCall* pending, int timeout) {
        // Reset response values to zeros and reset errors
        resetValues();

        // Do we have something pending?
        if (pending == NULL) {
            qDebug("QTEventLoop: pending call is NULL!");
            return;
        }

        MYDEBUGC("QTEventLoop: processing loop for %d ms", timeout);
        // If we have some pending operation, let's get notification about result
        dbus_pending_call_set_notify(pending, TestDbusQEventLoop::pendingNotify, this, NULL);

        // Setup timeout timer
        int activeTimer = startTimer(timeout);

        // Pump QT event loop, but only until pending operation is not finished
        while (!wasInNotifyFnc) {
            QCoreApplication::processEvents(QEventLoop::AllEvents);
            if (timerTimeout) {
                MYDEBUGC("QTEventLoop: timeout elapsed!!");
                return;
            }
        }

        killTimer(activeTimer);
    }

public:
    int			pongServerRunningSession;
    int			pongServerRunningSystem;
    int 		wasInNotifyFnc;
    const char* responseString;
    uint32_t	responseInt;
    bool		typeError;
    bool		noResponse;
    bool		timerTimeout;

    TestDbusQEventLoop() {
        resetValues();
    }

protected:
    void timerEvent(QTimerEvent *e) {
        // Set timeout flag and kill timer
        timerTimeout = true;
        killTimer(e->timerId());
    }

    static void pendingNotify(DBusPendingCall *pending, void *user_data) {
        MYDEBUG();
        // Get pointer to main class
        TestDbusQEventLoop* pThis = reinterpret_cast<TestDbusQEventLoop*>(user_data);
        // Get message from pending call
        DBusMessage* message = dbus_pending_call_steal_reply(pending);

        DBusMessageIter args;
        // Parse message
        if (!dbus_message_iter_init(message, &args))
            MYDEBUGC("Reply message has no arguments!");
        else {
            // Extract error message if is present
            const char* error = dbus_message_get_error_name(message);

            if (error != NULL) {
                // Check if it is NoReply message
                pThis->noResponse = (strcmp(error, "org.freedesktop.DBus.Error.NoReply") == 0);
            }

            // Get first argument
            int argType = dbus_message_iter_get_arg_type(&args);
            switch (argType) {
            case DBUS_TYPE_STRING:
                // Get string value
                dbus_message_iter_get_basic(&args, &pThis->responseString);

                // Display some debug info
                if (error != NULL) {
                    MYDEBUGC("Got error [%s]: %s", error, pThis->responseString);
                }
                else {
                    MYDEBUGC("Got Reply: %s", pThis->responseString);
                }
                break;
            case DBUS_TYPE_UINT32:
                // Handle integer
                dbus_message_iter_get_basic(&args, &pThis->responseInt);
                // Display some debug info
                MYDEBUGC("Got Reply: %d", pThis->responseInt);
                break;
            default:
                // Set unknown type flag
                pThis->typeError = true;
                // Display some debug info
                MYDEBUGC("Reply message argument has unsupported type (%d)!", argType);
            }
        }

        // Free message if needed
        if (message)
            dbus_message_unref(message);

        // Free pending call
        dbus_pending_call_unref(pending);
        // Set flag to end QEventLoop handler
        pThis->wasInNotifyFnc = 1;
    }

private slots:
    void initTestCase() {
        // First allocate and obtain
        systemBus = dbus_bus_get(DBUS_BUS_SYSTEM, NULL);
        sessionBus = dbus_bus_get(DBUS_BUS_SESSION, NULL);
        // Last check, if server is running
        pongServerRunningSystem = dbus_bus_name_has_owner(systemBus, "com.nokia.dbusqeventloop.test", NULL);
        pongServerRunningSession = dbus_bus_name_has_owner(sessionBus, "com.nokia.dbusqeventloop.test", NULL);


        // Create event loop ...
        QVERIFY(DBUSConnectionEventLoop::addConnection(systemBus) == true);
        QVERIFY(DBUSConnectionEventLoop::addConnection(sessionBus) == true);
        // Then test ... otherwise something don't have to be allocated (i.e. event loop)
        QVERIFY(systemBus != NULL);
        QVERIFY(sessionBus != NULL);
        QVERIFY(pongServerRunningSystem != 0);
        QVERIFY(pongServerRunningSession != 0);
    }

    void cleanupTestCase() {
        // Create "quit" method call message
        DBusMessage* message = dbus_message_new_method_call("com.nokia.dbusqeventloop.test", "/", NULL, "quit");
        DBusPendingCall* pending;
        QVERIFY(message != NULL);

        // Set correct values to suppress fake errors if connection fails in initTestCase()
        bool systemTimeout = false, sessionTimeout = false;
        int systemResponse = 12345, sessionResponse = 12345;

        // If pong server is running on system bus then send quit
        if (pongServerRunningSystem) {
            // Send the message
            dbus_connection_send_with_reply(systemBus, message, &pending, 1000);
            // Wait for response
            processQTEventLoop(pending, 4000);
            systemTimeout = timerTimeout;
            systemResponse = responseInt;
        }

        // If pong server is running on session bus then send quit
        if (pongServerRunningSession) {
            // Send the message
            dbus_connection_send_with_reply(sessionBus, message, &pending, 1000);
            // Wait for response
            processQTEventLoop(pending, 4000);
            sessionTimeout = timerTimeout;
            sessionResponse = responseInt;
        }

        // Free message, it is not necessary to test, QVERIFY upper will end function
        dbus_message_unref(message);

        // Check if everything went well
        QVERIFY(systemTimeout == false);
        QVERIFY(systemResponse == 12345);
        QVERIFY(sessionTimeout == false);
        QVERIFY(sessionResponse == 12345);
    }

    void pingSystemBusTest() {
        // Create message
        DBusMessage* message = dbus_message_new_method_call("com.nokia.dbusqeventloop.test", "/", NULL, "ping");
        QVERIFY(message != NULL);
        // Add argument to message
        const char* temp = "pekny kohutik co sa prechadza po svojom novom dvore a obzera si sliepocky";
        dbus_message_append_args(message, DBUS_TYPE_STRING, &temp, DBUS_TYPE_INVALID);

        DBusPendingCall* pending;
        // Send the message
        dbus_connection_send_with_reply(systemBus, message, &pending, 3000);

        // Free message
        dbus_message_unref(message);
        // Process event loop
        processQTEventLoop(pending, 4000);
        // Check results
        QVERIFY(timerTimeout == false);
        QVERIFY(typeError == false);
        QVERIFY(strcmp(temp, responseString) == 0);
    }

    void timeoutSystemBusTest() {
        // Create message
        DBusMessage* message = dbus_message_new_method_call("com.nokia.dbusqeventloop.test", "/", NULL, "timeout");
        QVERIFY(message != NULL);

        DBusPendingCall* pending;
        // Send the message
        dbus_connection_send_with_reply(systemBus, message, &pending, 1000);

        // Free message
        dbus_message_unref(message);
        // Process event loop
        processQTEventLoop(pending, 3000);
        // Check results
        QVERIFY(timerTimeout == false);
        QVERIFY(noResponse == true);
        QVERIFY(typeError == false);
        // Small pause to process reply
        sleep(1);
    }

    void pingSessionBusTest() {
        // Create message
        DBusMessage* message = dbus_message_new_method_call("com.nokia.dbusqeventloop.test", "/", NULL, "ping");
        QVERIFY(message != NULL);
        // Add argument to message
        const char* temp = "pekny kohutik co sa prechadza po svojom novom dvore a obzera si sliepocky";
        dbus_message_append_args(message, DBUS_TYPE_STRING, &temp, DBUS_TYPE_INVALID);

        DBusPendingCall* pending;
        // Send the message
        dbus_connection_send_with_reply(sessionBus, message, &pending, 3000);

        // Free message
        dbus_message_unref(message);
        // Process event loop
        processQTEventLoop(pending, 4000);
        // Check results
        QVERIFY(timerTimeout == false);
        QVERIFY(typeError == false);
        QVERIFY(strcmp(temp, responseString) == 0);
    }

    void timeoutSessionBusTest() {
        // Create message
        DBusMessage* message = dbus_message_new_method_call("com.nokia.dbusqeventloop.test", "/", NULL, "timeout");
        QVERIFY(message != NULL);

        DBusPendingCall* pending;
        // Send the message
        dbus_connection_send_with_reply(sessionBus, message, &pending, 1000);

        // Free message
        dbus_message_unref(message);
        // Process event loop
        processQTEventLoop(pending, 3000);
        // Check results
        QVERIFY(timerTimeout == false);
        QVERIFY(noResponse == true);
        QVERIFY(typeError == false);
        // Small pause to process reply
        sleep(1);
    }
};

QTEST_MAIN(TestDbusQEventLoop)
#include "test-dbus-qeventloop.moc"
