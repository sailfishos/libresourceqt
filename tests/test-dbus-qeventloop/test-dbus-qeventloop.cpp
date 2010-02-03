#include "dbusconnectioneventloop.h"
#include <QtTest/QtTest>

class TestDbusQEventLoop: public QObject
{
	Q_OBJECT

private slots:
	void basicTest()
	{
		DBusConnection* systemBus = dbus_bus_get(DBUS_BUS_SYSTEM, NULL);
		QVERIFY(systemBus != NULL);

		DBUSConnectionEventLoop d;
		QVERIFY(d.initConnection(systemBus) == true);
	}
};

QTEST_MAIN(TestDbusQEventLoop)
#include "test-dbus-qeventloop.moc"
