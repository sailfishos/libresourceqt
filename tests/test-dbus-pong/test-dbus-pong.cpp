#include <stdio.h>
#include <stdlib.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtDBus/QtDBus>

#include "pong.h"

QString Pong::ping(const QString &arg)
{
	// Just return back
	return QString("%1").arg(arg);
}

unsigned int Pong::quit()
{
	// Quit application
	QMetaObject::invokeMethod(QCoreApplication::instance(), "quit");
	// Magic number
	return 12345;
}

unsigned int Pong::timeout()
{
	// Timeout in testing application should be set to less than 2 seconds!
	sleep(2);
	// Just to suppress warning
	return 54321;
}

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);

	// Check system bus connection
	if( !QDBusConnection::systemBus().isConnected() )
	{
		qDebug("Cannot connect to the D-Bus system bus.");
		return 1;
	}

	// Create listener service
	if( !QDBusConnection::systemBus().registerService("com.nokia.dbusqeventloop.test") )
	{
		qDebug("%s", qPrintable(QDBusConnection::systemBus().lastError().message()));
		exit(2);
	}

	Pong pong;
	// Register all slots as dbus methods
	QDBusConnection::systemBus().registerObject("/", &pong, QDBusConnection::ExportAllSlots);

	// Let's go!
	app.exec();

	return 0;
}
