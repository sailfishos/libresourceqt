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
    bool useSessionBus = false;

    QCoreApplication app(argc, argv);
    if (app.arguments().count() > 1) {
        if (app.arguments().at(1) == "--session") {
            useSessionBus = true;
        }
    }

    QDBusConnection* myBus;
    if (useSessionBus) {
        myBus = new QDBusConnection(QDBusConnection::sessionBus().sessionBus());
        qDebug("Using session bus ...");
    }
    else {
        myBus = new QDBusConnection(QDBusConnection::systemBus().systemBus());
        qDebug("Using system bus ...");
    }

    // Check system bus connection
    if (!myBus->isConnected()) {
        // Cleanup!!
        delete myBus;
        myBus = NULL;

        if (useSessionBus) {
            qDebug("Cannot connect to the D-Bus session bus.");
        }
        else {
            qDebug("Cannot connect to the D-Bus system bus.");
        }

        return 1;
    }

    // Create listener service
    if (!myBus->registerService("com.nokia.dbusqeventloop.test")) {
        qDebug("%s", qPrintable(QDBusConnection::systemBus().lastError().message()));
        exit(2);
    }

    Pong pong;
    // Register all slots as dbus methods
    myBus->registerObject("/", &pong, QDBusConnection::ExportAllSlots);

    // Let's go!
    app.exec();

    // Cleanup!!
    delete myBus;
    myBus = NULL;

    return 0;
}
