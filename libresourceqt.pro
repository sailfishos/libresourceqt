TEMPLATE = lib
VERSION = 1.0.0
TARGET = resourceqt
DESTDIR = build
DEPENDPATH += include src
INCLUDEPATH += src include

# Input
PUBLIC_HEADERS = include/resource.h

HEADERS += $$PUBLIC_HEADERS

SOURCES += src/resource.cpp

OBJECTS_DIR = build
MOC_DIR = build

CONFIG  += qt link_pkgconfig dll
QT = core dbus
PKGCONFIG += dbus-1

# Install directives
headers.files  = $$PUBLIC_HEADERS
INSTALLBASE    = /usr
target.path    = $$INSTALLBASE/lib
headers.path   = $$INSTALLBASE/include/resource/qt4

INSTALLS       = target headers
