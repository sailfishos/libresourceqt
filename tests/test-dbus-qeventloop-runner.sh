#!/bin/sh

# Source and export D-Bus session info 
. /tmp/session_bus_address.user 

# Run pong server on system bus
/usr/lib/libresourceqt-tests/test-dbus-pong &

# Run pong server on system bus
/usr/lib/libresourceqt-tests/test-dbus-pong --session &

sleep 2

# do the testing!
/usr/lib/libresourceqt-tests/test-dbus-qeventloop
