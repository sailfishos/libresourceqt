#-------------------------------------------------
#
# Project created by QtCreator 2010-01-18T16:28:34
#
#-------------------------------------------------


TARGET      = dbus-qeventloop
TEMPLATE    = lib
VERSION     = 0.1
DESTDIR     = build
MOC_DIR     = .moc
OBJECTS_DIR = .obj

DEFINES    += LIBDBUSQEVENTLOOP_LIBRARY
SOURCES    += dbusconnectioneventloop.cpp

HEADERS    += dbusconnectioneventloop.h

QT          = core
CONFIG     += qt link_pkgconfig dll
PKGCONFIG  += dbus-1

# Install directives
headers.files  = $$HEADERS
INSTALLBASE    = /usr
target.path    = $$INSTALLBASE/lib
headers.path   = $$INSTALLBASE/include/resource/qt4
INSTALLS       = target headers
