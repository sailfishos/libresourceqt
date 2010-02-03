#include <stdio.h>

#include <QtCore/QCoreApplication>
#include <QtDBus/QtDBus>

#define SERVICE_NAME            "com.nokia.dbusqeventloop.test"

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

	QDBusInterface iface(SERVICE_NAME, "/", "", QDBusConnection::systemBus());
	if( iface.isValid() )
	{
		QDBusReply<QString> reply = iface.call("ping", argc > 1 ? argv[1] : "");
		if( reply.isValid() )
		{
			printf("Reply was: %s\n", qPrintable(reply.value()));
			return 0;
		}

		fprintf(stderr, "Call failed: %s\n", qPrintable(reply.error().message()));
		return 2;
	}

	fprintf(stderr, "%s\n", qPrintable(QDBusConnection::systemBus().lastError().message()));
	return 3;
}
