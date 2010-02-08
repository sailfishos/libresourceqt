#!/bin/sh

# Source and export D-Bus session info 
. /tmp/dbus-info
export DBUS_SESSION_BUS_ADDRESS
export DBUS_SESSION_BUS_PID
export DBUS_SESSION_BUS_WINDOWID

# Run pong server on system bus
/usr/lib/libresourceqt-tests/test-dbus-pong &

# Run pong server on system bus
/usr/lib/libresourceqt-tests/test-dbus-pong --session &

sleep 2

# do the testing!
/usr/lib/libresourceqt-tests/test-dbus-qeventloop
