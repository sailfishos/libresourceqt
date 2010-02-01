TEMPLATE = lib
VERSION = 1.0.0
TARGET = resourceqt
DESTDIR = build
DEPENDPATH += include src
INCLUDEPATH += src include ../libresource/src

# Input
PUBLIC_HEADERS = include/resource.h include/resource-set.h include/resources.h

HEADERS += $$PUBLIC_HEADERS

SOURCES += src/resource.cpp \
           src/resource-set.cpp \
           src/resources.cpp

OBJECTS_DIR = build
MOC_DIR = build

CONFIG  += qt link_pkgconfig dll
QT = core
PKGCONFIG += dbus-1

# Install directives
headers.files  = $$PUBLIC_HEADERS
INSTALLBASE    = /usr
target.path    = $$INSTALLBASE/lib
headers.path   = $$INSTALLBASE/include/resource/qt4

INSTALLS       = target headers

