#include <stdio.h>
#include <stdlib.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtDBus/QtDBus>

#include "pong.h"

#define SERVICE_NAME            "com.nokia.dbusqeventloop.test"

QString Pong::ping(const QString &arg)
{
	QMetaObject::invokeMethod(QCoreApplication::instance(), "quit");
	return QString("ping(\"%1\") got called").arg(arg);
}

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);

	if( !QDBusConnection::systemBus().isConnected() )
	{
		fprintf(stderr, "Cannot connect to the D-Bus system bus.\n"
			"To start it, run:\n"
			"\teval `dbus-launch --auto-syntax`\n");
		return 1;
	}

	if( !QDBusConnection::systemBus().registerService(SERVICE_NAME) )
	{
		fprintf(stderr, "%s\n", qPrintable(QDBusConnection::systemBus().lastError().message()));
		exit(2);
	}

	Pong pong;
	QDBusConnection::systemBus().registerObject("/", &pong, QDBusConnection::ExportAllSlots);

	app.exec();
	return 0;
}
