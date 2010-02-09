include(../../common.pri)
TEMPLATE = app
TARGET = test-resource
DESTDIR = build
DEPENDPATH += $${POLICY} $${LIBRESOURCEQT}/src .
INCLUDEPATH += $${LIBRESOURCEQT}/src $${LIBRESOURCEINC}

# Input
HEADERS +=  $${POLICY}/resource.h $${POLICY}/resources.h test-resource.h
SOURCES +=  $${LIBRESOURCEQT}/src/resource.cpp $${LIBRESOURCEQT}/src/resources.cpp test-resource.cpp

OBJECTS_DIR = build
MOC_DIR = build

CONFIG  += qt qtestlib debug warn_on
QT -= gui

# Install directives
INSTALLBASE    = /usr
target.path    = $${INSTALLBASE}/lib/libresourceqt-tests/
INSTALLS       = target
