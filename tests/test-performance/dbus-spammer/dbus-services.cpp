#include "dbus-services.h"

DbusSpammer::DbusSpammer(int id, QString service, int messageLen) : QThread()
{
    firstTime = true;
//    qDebug("Hello world from DbusSpammer::DbusSpammer(%d, %s, %d)", id, qPrintable(service), messageLen);

    myBus = dbus_bus_get(DBUS_BUS_SESSION, NULL);
    if( myBus == NULL )
    {
        qDebug("Failed to access session bus!");
        return;
    }

    threadID    = id;
    serviceName = service;
    spamData.fill('A', messageLen);
    pszSpamData = qPrintable(spamData);

    myMessage = dbus_message_new_method_call(qPrintable(service), "/", NULL, "ping");
    if( myMessage == NULL )
    {
        qDebug("Failed to create dbus message! Out of memory?");
        return;
    }

    dbus_message_append_args(myMessage, DBUS_TYPE_STRING, &pszSpamData, DBUS_TYPE_INVALID);

    timerID = startTimer(0);
}

DbusSpammer::~DbusSpammer()
{
    killTimer(timerID);

    dbus_message_unref(myMessage);
    dbus_connection_unref(myBus);
}

void DbusSpammer::run()
{
//    sleep(1);
//    printf("Thread #%d: Using service '%s' ...\n", threadID, qPrintable(serviceName));

    exec();
}

void DbusSpammer::timerEvent(QTimerEvent*)
{
    killTimer(timerID);

    if( firstTime )
    {
        dbus_connection_send(myBus, myMessage, NULL);
        dbus_connection_flush(myBus);
    }
    else
    {
        DBusError error;
        dbus_error_init(&error);
        DBusMessage* reply = dbus_connection_send_with_reply_and_block(myBus, myMessage, 1000, &error);

        if( dbus_error_is_set(&error) )
        {
            if( QString(error.name) != QString("org.freedesktop.DBus.Error.AccessDenied") )
                qDebug("Thread #%d dbus error %s:\n  %s", threadID, error.name, error.message);
        }

        if( reply )
        {
            dbus_message_unref(reply);
        }
    }

    firstTime = false;
    timerID = startTimer(0);
}

DbusServer::DbusServer(int id)
{
    serviceID = id;
    serviceName.sprintf("com.nokia.spam.dbus%d", id);
}

DbusServer::~DbusServer()
{
    if( myBus )
    {
        delete myBus;
        myBus = NULL;
    }
}

void DbusServer::run()
{
    myBus = new QDBusConnection(QDBusConnection::sessionBus().sessionBus());
    if( !myBus || !myBus->isConnected() )
    {
        qDebug("Connection error!");
        return;
    }

    if( !myBus->registerService(serviceName) )
    {
        qDebug("Service #%d: %s", serviceID, qPrintable(myBus->lastError().message()));
        return;
    }

    // Register all slots as dbus methods
    myBus->registerObject("/", this, QDBusConnection::ExportScriptableSlots);

//    qDebug("DbusServer(%d) running!", serviceID);
    exec();
}

QString DbusServer::ping(const QString &arg)
{
    // Just return back
    return QString("%1").arg(arg);
}
