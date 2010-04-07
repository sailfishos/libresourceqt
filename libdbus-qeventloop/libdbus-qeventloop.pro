
include(../common.pri)
TARGET      = dbus-qeventloop
TEMPLATE    = lib
DESTDIR     = build
MOC_DIR     = .moc
OBJECTS_DIR = .obj

DEFINES    += LIBDBUSQEVENTLOOP_LIBRARY
SOURCES    += dbusconnectioneventloop.cpp

HEADERS    += dbusconnectioneventloop.h

QT          = core
CONFIG     += qt link_pkgconfig dll
PKGCONFIG  += dbus-1
DEFINES    += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT QT_NO_DEBUG_STREAM

# Install directives
headers.files  = $$HEADERS
INSTALLBASE    = /usr
target.path    = $$INSTALLBASE/lib
headers.path   = $$INSTALLBASE/include
pc.files       = libdbus-qeventloop1.pc
pc.path        = $${INSTALLBASE}/lib/pkgconfig
INSTALLS       = target headers pc

