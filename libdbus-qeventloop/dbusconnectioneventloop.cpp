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

#include <QCoreApplication>
#include <QSocketNotifier>
#include <QTimer>
#include <QTimerEvent>

#include "dbusconnectioneventloop.h"

Q_GLOBAL_STATIC(DBUSConnectionEventLoop, classInstance);

bool DBUSConnectionEventLoop::addConnection(DBusConnection* conn)
{
    return classInstance()->internalAddConnection(conn);
}

void DBUSConnectionEventLoop::removeConnection(DBusConnection* conn)
{
    classInstance()->internalRemoveConnection(conn);
}

DBUSConnectionEventLoop::DBUSConnectionEventLoop() : QObject()
{
    MYDEBUG();
}

DBUSConnectionEventLoop::~DBUSConnectionEventLoop()
{
    MYDEBUG();
    cleanup();
}

void DBUSConnectionEventLoop::cleanup()
{
    MYDEBUG();

    Connections::iterator it;
    for (it = connections.begin(); it != connections.end(); ++it) {
        MYDEBUGC("Cleaning connection %p", *it);

        dbus_connection_set_watch_functions(*it, NULL, NULL, NULL, NULL, NULL);
        dbus_connection_set_timeout_functions(*it, NULL, NULL, NULL, NULL, NULL);
        dbus_connection_set_wakeup_main_function(*it, NULL, NULL, NULL);
    }
}

// Handle a socket being ready to read.
void DBUSConnectionEventLoop::readSocket(int fd)
{
    MYDEBUG();

    Watchers::const_iterator it = watchers.find(fd);

    while (it != watchers.end() && it.key() == fd) {
        const Watcher &watcher = it.value();

        if (watcher.read && watcher.read->isEnabled()) {
            dbus_watch_handle(watcher.watch, DBUS_WATCH_READABLE);
            break;
        }

        ++it;
    }

    dispatch();
}

// Handle a socket being ready to write.
void DBUSConnectionEventLoop::writeSocket(int fd)
{
    MYDEBUG();

    Watchers::const_iterator it = watchers.find(fd);

    while (it != watchers.end() && it.key() == fd) {
        const Watcher &watcher = it.value();

        if (watcher.write && watcher.write->isEnabled()) {
            dbus_watch_handle(watcher.watch, DBUS_WATCH_WRITABLE);
            break;
        }

        ++it;
    }
}

void DBUSConnectionEventLoop::dispatch()
{
    MYDEBUG();

    for (Connections::const_iterator it = connections.constBegin(); it != connections.constEnd(); ++it)
        while (dbus_connection_dispatch(*it) == DBUS_DISPATCH_DATA_REMAINS)
            ;
}

// Handle timer events.
void DBUSConnectionEventLoop::timerEvent(QTimerEvent *e)
{
    MYDEBUG();
    MYDEBUGC("TimerID: %d", e->timerId());

    DBusTimeout *timeout = timeouts.value(e->timerId());

    if (timeout)
        dbus_timeout_handle(timeout);
}

dbus_bool_t DBUSConnectionEventLoop::addWatch(DBusWatch *watch, void *data)
{
    MYDEBUG();

    DBUSConnectionEventLoop *loop = reinterpret_cast<DBUSConnectionEventLoop *>(data);

    int fd = dbus_watch_get_unix_fd(watch);
    unsigned int flags = dbus_watch_get_flags(watch);
    dbus_bool_t enabled = dbus_watch_get_enabled(watch);

    DBUSConnectionEventLoop::Watcher watcher;
    watcher.watch = watch;

    if (flags & DBUS_WATCH_READABLE) {
        watcher.read = new QSocketNotifier(fd, QSocketNotifier::Read, loop);
        watcher.read->setEnabled(enabled);
        loop->connect(watcher.read, SIGNAL(activated(int)), SLOT(readSocket(int)));
    }

    if (flags & DBUS_WATCH_WRITABLE) {
        watcher.write = new QSocketNotifier(fd, QSocketNotifier::Write, loop);
        watcher.write->setEnabled(enabled);
        loop->connect(watcher.write, SIGNAL(activated(int)), SLOT(writeSocket(int)));
    }

    loop->watchers.insertMulti(fd, watcher);

    return true;
}

void DBUSConnectionEventLoop::removeWatch(DBusWatch *watch, void *data)
{
    MYDEBUG();

    DBUSConnectionEventLoop *loop = reinterpret_cast<DBUSConnectionEventLoop *>(data);

    int fd = dbus_watch_get_unix_fd(watch);

    DBUSConnectionEventLoop::Watchers::iterator it = loop->watchers.find(fd);

    while (it != loop->watchers.end() && it.key() == fd) {
        DBUSConnectionEventLoop::Watcher &watcher = it.value();

        if (watcher.watch == watch) {
            if (watcher.read) {
                delete watcher.read;
            }
            if (watcher.write) {
                delete watcher.write;
            }

            loop->watchers.erase(it);

            return;
        }

        ++it;
    }
}

void DBUSConnectionEventLoop::toggleWatch(DBusWatch *watch, void *data)
{
    MYDEBUG();

    DBUSConnectionEventLoop *loop = reinterpret_cast<DBUSConnectionEventLoop*>(data);

    int fd = dbus_watch_get_unix_fd(watch);
    unsigned int flags = dbus_watch_get_flags(watch);
    dbus_bool_t enabled = dbus_watch_get_enabled(watch);

    DBUSConnectionEventLoop::Watchers::const_iterator it = loop->watchers.find(fd);

    while (it != loop->watchers.end() && it.key() == fd) {
        const DBUSConnectionEventLoop::Watcher &watcher = it.value();

        if (watcher.watch == watch) {
            if (flags & DBUS_WATCH_READABLE && watcher.read)
                watcher.read->setEnabled(enabled);

            if (flags & DBUS_WATCH_WRITABLE && watcher.write)
                watcher.write->setEnabled(enabled);

            return;
        }

        ++it;
    }
}

dbus_bool_t DBUSConnectionEventLoop::addTimeout(DBusTimeout *timeout, void *data)
{
    MYDEBUG();

    // Nothing to do if the timeout is disabled.
    if (!dbus_timeout_get_enabled(timeout))
        return true;

    // Pretend it is successful if there is no application instance.
    if (!QCoreApplication::instance())
        return true;

    DBUSConnectionEventLoop *loop = reinterpret_cast<DBUSConnectionEventLoop *>(data);

    int timerInterval = dbus_timeout_get_interval(timeout);
    int id = loop->startTimer(timerInterval);

    MYDEBUGC("Adding timeout %d with interval %d!", id, timerInterval);

    if (!id)
        return false;

    loop->timeouts[id] = timeout;

    return true;
}

void DBUSConnectionEventLoop::removeTimeout(DBusTimeout *timeout, void *data)
{
    MYDEBUG();

    DBUSConnectionEventLoop *loop = reinterpret_cast<DBUSConnectionEventLoop *>(data);

    DBUSConnectionEventLoop::Timeouts::iterator it = loop->timeouts.begin();

    while (it != loop->timeouts.end()) {
        if (it.value() == timeout) {
            loop->killTimer(it.key());
            it = loop->timeouts.erase(it);
        }
        else
            ++it;
    }
}

void DBUSConnectionEventLoop::toggleTimeout(DBusTimeout *timeout, void *data)
{
    MYDEBUG();

    DBUSConnectionEventLoop::removeTimeout(timeout, data);
    DBUSConnectionEventLoop::addTimeout(timeout, data);
}

void DBUSConnectionEventLoop::wakeupMain(void *data)
{
    MYDEBUG();

    DBUSConnectionEventLoop *loop = reinterpret_cast<DBUSConnectionEventLoop *>(data);

    QTimer::singleShot(0, loop, SLOT(dispatch()));
}

// The initialization point
bool DBUSConnectionEventLoop::internalAddConnection(DBusConnection* conn)
{
    MYDEBUG();

    bool rc = true;

    if (conn == NULL) {
        return false;
    }

    MYDEBUGC("Adding connection %p", conn);

    // Check if connection is in list
    Connections::iterator it;
    for (it = connections.begin(); it != connections.end(); ++it) {
        if( *it == conn ) {
            MYDEBUGC("Connection already in list, skipping");
            // Skip adding duplicate connection
            return true;
        }
    }
    // Add new connection
    connections.append(conn);

    if (
        !dbus_connection_set_watch_functions(conn,
                                             DBUSConnectionEventLoop::addWatch,
                                             DBUSConnectionEventLoop::removeWatch,
                                             DBUSConnectionEventLoop::toggleWatch,
                                             this, 0)
    ) {
        rc = false;
    }
    else if (
        !dbus_connection_set_timeout_functions(conn,
                                               DBUSConnectionEventLoop::addTimeout,
                                               DBUSConnectionEventLoop::removeTimeout,
                                               DBUSConnectionEventLoop::toggleTimeout,
                                               this, 0)
    ) {
        rc = false;
    }
    else {
        rc = true;
    }

    dbus_connection_set_wakeup_main_function(conn, DBUSConnectionEventLoop::wakeupMain, this, 0);

    return rc;
}

void DBUSConnectionEventLoop::internalRemoveConnection(DBusConnection* conn)
{
    MYDEBUG();

    Connections::iterator it;
    for (it = connections.begin(); it != connections.end(); ++it) {
        if( *it == conn ) {
            dbus_connection_set_watch_functions(*it, NULL, NULL, NULL, NULL, NULL);
            dbus_connection_set_timeout_functions(*it, NULL, NULL, NULL, NULL, NULL);
            dbus_connection_set_wakeup_main_function(*it, NULL, NULL, NULL);

            connections.erase(it);
            return;
        }
    }
}
