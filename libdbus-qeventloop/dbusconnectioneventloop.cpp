#include <QCoreApplication>
#include <QSocketNotifier>
#include <QTimer>
#include <QTimerEvent>

#include "dbusconnectioneventloop.h"
#include <stdio.h>

DBUSConnectionEventLoop::DBUSConnectionEventLoop() : QObject()
{
}

DBUSConnectionEventLoop::~DBUSConnectionEventLoop()
{
}

// Handle a socket being ready to read.
void DBUSConnectionEventLoop::readSocket(int fd)
{
    Watchers::const_iterator it = watchers.find(fd);

    while( it != watchers.end() && it.key() == fd )
    {
        const Watcher &watcher = it.value();

        if( watcher.read && watcher.read->isEnabled() )
        {
            watcher.read->setEnabled(false);
            dbus_watch_handle(watcher.watch, DBUS_WATCH_READABLE);
            watcher.read->setEnabled(true);
            break;
        }

        ++it;
    }

    dispatch();
}

// Handle a socket being ready to write.
void DBUSConnectionEventLoop::writeSocket(int fd)
{
    Watchers::const_iterator it = watchers.find(fd);

    while( it != watchers.end() && it.key() == fd )
    {
        const Watcher &watcher = it.value();

        if( watcher.write && watcher.write->isEnabled() )
        {
            watcher.write->setEnabled(false);
            dbus_watch_handle(watcher.watch, DBUS_WATCH_WRITABLE);
            watcher.write->setEnabled(true);
            break;
        }

        ++it;
    }
}

void DBUSConnectionEventLoop::dispatch()
{
    for( Connections::const_iterator it = connections.constBegin(); it != connections.constEnd(); ++it )
        while( dbus_connection_dispatch(*it) == DBUS_DISPATCH_DATA_REMAINS )
            ;
}

// Handle timer events.
void DBUSConnectionEventLoop::timerEvent(QTimerEvent *e)
{
    DBusTimeout *timeout = timeouts.value(e->timerId());

    if( timeout )
        dbus_timeout_handle(timeout);
}

dbus_bool_t DBUSConnectionEventLoop::addWatch(DBusWatch *watch, void *data)
{
	DBUSConnectionEventLoop *loop = reinterpret_cast<DBUSConnectionEventLoop *>(data);

    int fd = dbus_watch_get_unix_fd(watch);
    unsigned int flags = dbus_watch_get_flags(watch);
    dbus_bool_t enabled = dbus_watch_get_enabled(watch);

    DBUSConnectionEventLoop::Watcher watcher;
    watcher.watch = watch;

    if( flags & DBUS_WATCH_READABLE )
    {
        watcher.read = new QSocketNotifier(fd, QSocketNotifier::Read, loop);
        watcher.read->setEnabled(enabled);
        loop->connect(watcher.read, SIGNAL(activated(int)), SLOT(readSocket(int)));
    }

    if( flags & DBUS_WATCH_WRITABLE )
    {
        watcher.write = new QSocketNotifier(fd, QSocketNotifier::Write, loop);
        watcher.write->setEnabled(enabled);
        loop->connect(watcher.write, SIGNAL(activated(int)), SLOT(writeSocket(int)));
    }

    loop->watchers.insertMulti(fd, watcher);

    return true;
}

void DBUSConnectionEventLoop::removeWatch(DBusWatch *watch, void *data)
{
	DBUSConnectionEventLoop *loop = reinterpret_cast<DBUSConnectionEventLoop *>(data);

    int fd = dbus_watch_get_unix_fd(watch);

    DBUSConnectionEventLoop::Watchers::iterator it = loop->watchers.find(fd);

    while( it != loop->watchers.end() && it.key() == fd )
    {
    	DBUSConnectionEventLoop::Watcher &watcher = it.value();

        if( watcher.watch == watch )
        {
            if( watcher.read )
                delete watcher.read;

            if( watcher.write )
                delete watcher.write;

            loop->watchers.erase(it);

            return;
        }

        ++it;
    }
}

void DBUSConnectionEventLoop::toggleWatch(DBusWatch *watch, void *data)
{
	DBUSConnectionEventLoop *loop = reinterpret_cast<DBUSConnectionEventLoop *>(data);

    int fd = dbus_watch_get_unix_fd(watch);
    unsigned int flags = dbus_watch_get_flags(watch);
    dbus_bool_t enabled = dbus_watch_get_enabled(watch);

    DBUSConnectionEventLoop::Watchers::const_iterator it = loop->watchers.find(fd);

    while( it != loop->watchers.end() && it.key() == fd )
    {
        const DBUSConnectionEventLoop::Watcher &watcher = it.value();

        if( watcher.watch == watch )
        {
            if( flags & DBUS_WATCH_READABLE && watcher.read )
                watcher.read->setEnabled(enabled);

            if( flags & DBUS_WATCH_WRITABLE && watcher.write )
                watcher.write->setEnabled(enabled);

            return;
        }

        ++it;
    }
}

dbus_bool_t DBUSConnectionEventLoop::addTimeout(DBusTimeout *timeout, void *data)
{
    // Nothing to do if the timeout is disabled.
    if( !dbus_timeout_get_enabled(timeout) )
        return true;

    // Pretend it is successful if there is no application instance.
    if( !QCoreApplication::instance() )
        return true;

    DBUSConnectionEventLoop *loop = reinterpret_cast<DBUSConnectionEventLoop *>(data);

    int id = loop->startTimer(dbus_timeout_get_interval(timeout));

    if( !id )
        return false;

    loop->timeouts[id] = timeout;

    return true;
}

void DBUSConnectionEventLoop::removeTimeout(DBusTimeout *timeout, void *data)
{
	DBUSConnectionEventLoop *loop = reinterpret_cast<DBUSConnectionEventLoop *>(data);

	DBUSConnectionEventLoop::Timeouts::iterator it = loop->timeouts.begin();

    while( it != loop->timeouts.end() )
    {
        if( it.value() == timeout )
        {
            loop->killTimer(it.key());
            it = loop->timeouts.erase(it);
        }
        else
            ++it;
    }
}

void DBUSConnectionEventLoop::toggleTimeout(DBusTimeout *timeout, void *data)
{
	DBUSConnectionEventLoop::removeTimeout(timeout, data);
	DBUSConnectionEventLoop::addTimeout(timeout, data);
}

void DBUSConnectionEventLoop::wakeupMain(void *data)
{
	DBUSConnectionEventLoop *loop = reinterpret_cast<DBUSConnectionEventLoop *>(data);

    QTimer::singleShot(0, loop, SLOT(dispatch()));
}

// The initialization point
bool DBUSConnectionEventLoop::initConnection(DBusConnection* conn)
{
    bool rc;

	if( conn == NULL )
	{
		return false;
	}

    connections.append(conn);

    if(
    	!dbus_connection_set_watch_functions(conn,
    			DBUSConnectionEventLoop::addWatch,
    			DBUSConnectionEventLoop::removeWatch,
    			DBUSConnectionEventLoop::toggleWatch,
    			this, 0)
    	)
    {
        rc = false;
    }
    else if(
    	!dbus_connection_set_timeout_functions(conn,
    			DBUSConnectionEventLoop::addTimeout,
    			DBUSConnectionEventLoop::removeTimeout,
    			DBUSConnectionEventLoop::toggleTimeout,
    			this, 0)
    		)
    {
        rc = false;
    }
    else
    {
        rc = true;
    }

    dbus_connection_set_wakeup_main_function(conn, DBUSConnectionEventLoop::wakeupMain, this, 0);

    return rc;
}

