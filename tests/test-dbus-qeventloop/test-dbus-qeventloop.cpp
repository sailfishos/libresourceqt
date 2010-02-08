#include "dbusconnectioneventloop.h"
#include <QtTest/QtTest>

class TestDbusQEventLoop: public QObject
{
	Q_OBJECT

private:
	DBusConnection* systemBus;
	DBusConnection* sessionBus;
	DBUSConnectionEventLoop* dbusEventLoop;

	void resetValues()
	{
		wasInNotifyFnc = 0;
		responseString = 0;
		responseInt    = 0;
		typeError      = false;
		noResponse     = false;
    	timerTimeout   = false;
	}

	void processQTEventLoop(DBusPendingCall* pending, int timeout)
	{
		// Reset response values to zeros and reset errors
		resetValues();

		// Do we have something pending?
		if( pending == NULL )
		{
			return;
		}

		// If we have some pending operation, let's get notification about result
		dbus_pending_call_set_notify(pending, TestDbusQEventLoop::pendingNotify, this, NULL);

		// Setup timeout timer
		startTimer(timeout);

		// Pump QT event loop, but only until pending operation is not finished
		while( !wasInNotifyFnc )
		{
			QCoreApplication::processEvents(QEventLoop::AllEvents);
			if( timerTimeout )
			{
				return;
			}
		}
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

	TestDbusQEventLoop()
	{
		resetValues();
	}

protected:
    void timerEvent(QTimerEvent *e)
    {
    	timerTimeout = true;
    	killTimer(e->timerId());
    }

	static void pendingNotify(DBusPendingCall *pending, void *user_data)
	{
		MYDEBUG();
		TestDbusQEventLoop* pThis = reinterpret_cast<TestDbusQEventLoop*>(user_data);
		DBusMessage* message = dbus_pending_call_steal_reply(pending);

		DBusMessageIter args;
		if( !dbus_message_iter_init(message, &args) )
			MYDEBUGC("Reply message has no arguments!");
		else
		{
			const char* error = dbus_message_get_error_name(message);

			if( error != NULL )
			{
				pThis->noResponse = (strcmp(error, "org.freedesktop.DBus.Error.NoReply") == 0);
			}

			int argType = dbus_message_iter_get_arg_type(&args);
			switch( argType )
			{
			case DBUS_TYPE_STRING:
				dbus_message_iter_get_basic(&args, &pThis->responseString);
				if( error != NULL )
				{
					MYDEBUGC("Got error [%s]: %s", error, pThis->responseString);
				}
				else
				{
					MYDEBUGC("Got Reply: %s", pThis->responseString);
				}
				break;
			case DBUS_TYPE_BOOLEAN:
			case DBUS_TYPE_UINT32:
				dbus_message_iter_get_basic(&args, &pThis->responseInt);
				MYDEBUGC("Got Reply: %d", pThis->responseInt);
				break;
			default:
				MYDEBUGC("Reply message argument has unsupported type (%d)!", argType);
				pThis->typeError = true;
			}
		}

		if( message )
			dbus_message_unref(message);

		dbus_pending_call_unref(pending);

		pThis->wasInNotifyFnc = 1;
	}

private slots:
	void initTestCase()
	{
		// First allocate and obtain
		dbusEventLoop = new DBUSConnectionEventLoop();
		systemBus = dbus_bus_get(DBUS_BUS_SYSTEM, NULL);
		sessionBus = dbus_bus_get(DBUS_BUS_SESSION, NULL);;
		// Last check, if server is running
		pongServerRunningSystem = dbus_bus_name_has_owner(systemBus, "com.nokia.dbusqeventloop.test", NULL);
		pongServerRunningSession = dbus_bus_name_has_owner(sessionBus, "com.nokia.dbusqeventloop.test", NULL);


		// Create event loop ...
		QVERIFY(dbusEventLoop != NULL);
		QVERIFY(dbusEventLoop->addConnection(systemBus) == true);
		QVERIFY(dbusEventLoop->addConnection(sessionBus) == true);
		// Then test ... otherwise something don't have to be allocated (i.e. event loop)
		QVERIFY(systemBus != NULL);
		QVERIFY(sessionBus != NULL);
		QVERIFY(pongServerRunningSystem != 0);
		QVERIFY(pongServerRunningSession != 0);
	}

	void cleanupTestCase()
	{
		DBusMessage* message = dbus_message_new_method_call("com.nokia.dbusqeventloop.test", "/", NULL, "quit");
		DBusPendingCall* pending;
		QVERIFY(message != NULL);

		// Set correct values to suppress fake errors if connection fails in initTestCase()
		bool systemTimeout = false, sessionTimeout = false;
		int systemResponse = 12345, sessionResponse = 12345;

		if( pongServerRunningSystem )
		{
			dbus_connection_send_with_reply(systemBus, message, &pending, 1000);

			processQTEventLoop(pending, 4000);
			systemTimeout = timerTimeout;
			systemResponse = responseInt;
		}

		if( pongServerRunningSession )
		{
			dbus_connection_send_with_reply(sessionBus, message, &pending, 1000);

			processQTEventLoop(pending, 4000);
			sessionTimeout = timerTimeout;
			sessionResponse = responseInt;
		}

		if( message )
		{
			dbus_message_unref(message);
		}

		QVERIFY(dbusEventLoop != NULL);
		if( dbusEventLoop )
		{
			delete dbusEventLoop;
			dbusEventLoop = NULL;
		}

		QVERIFY(systemTimeout == false);
		QVERIFY(systemResponse == 12345);
		QVERIFY(sessionTimeout == false);
		QVERIFY(sessionResponse == 12345);
	}

	void pingSystemBusTest()
	{
		DBusMessage* message = dbus_message_new_method_call("com.nokia.dbusqeventloop.test", "/", NULL, "ping");
		QVERIFY(message != NULL);

		const char* temp = "pekny kohutik co sa prechadza po svojom novom dvore a obzera si sliepocky";
		dbus_message_append_args(message, DBUS_TYPE_STRING, &temp, DBUS_TYPE_INVALID);

		DBusPendingCall* pending;
        dbus_connection_send_with_reply(systemBus, message, &pending, 3000);

		// Free the signal now we have finished with it
		dbus_message_unref(message);

		processQTEventLoop(pending, 4000);
		QVERIFY(timerTimeout == false);
		QVERIFY(strcmp(temp, responseString) == 0);
	}
/*
	void timeoutSystemBusTest()
	{
		DBusMessage* message = dbus_message_new_method_call("com.nokia.dbusqeventloop.test", "/", NULL, "timeout");
		QVERIFY(message != NULL);

		DBusPendingCall* pending;
        dbus_connection_send_with_reply(systemBus, message, &pending, 1000);

		// Free the signal now we have finished with it
		dbus_message_unref(message);

		processQTEventLoop(pending, 4000);
		QVERIFY(timerTimeout == false);
		QVERIFY(noResponse == true);
		sleep(1);
	}
*/
	void pingSessionBusTest()
	{
		DBusMessage* message = dbus_message_new_method_call("com.nokia.dbusqeventloop.test", "/", NULL, "ping");
		QVERIFY(message != NULL);

		const char* temp = "pekny kohutik co sa prechadza po svojom novom dvore a obzera si sliepocky";
		dbus_message_append_args(message, DBUS_TYPE_STRING, &temp, DBUS_TYPE_INVALID);

		DBusPendingCall* pending;
        dbus_connection_send_with_reply(sessionBus, message, &pending, 3000);

		// Free the signal now we have finished with it
		dbus_message_unref(message);

		processQTEventLoop(pending, 4000);
		QVERIFY(timerTimeout == false);
		QVERIFY(strcmp(temp, responseString) == 0);
	}

	void timeoutSessionBusTest()
	{
		DBusMessage* message = dbus_message_new_method_call("com.nokia.dbusqeventloop.test", "/", NULL, "timeout");
		QVERIFY(message != NULL);

		DBusPendingCall* pending;
        dbus_connection_send_with_reply(sessionBus, message, &pending, 1000);

		// Free the signal now we have finished with it
		dbus_message_unref(message);

		processQTEventLoop(pending, 4000);
		QVERIFY(timerTimeout == false);
		QVERIFY(noResponse == true);
		sleep(1);
	}
};

QTEST_MAIN(TestDbusQEventLoop)
#include "test-dbus-qeventloop.moc"
