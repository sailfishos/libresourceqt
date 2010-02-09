include(../../common.pri)
TEMPLATE = app
TARGET = test-resource-set
DESTDIR = build
DEPENDPATH += $${POLICY} $${BASE}/src .
INCLUDEPATH += $${LIBRESOURCEQT}/src $${LIBRESOURCEINC}

# Input
HEADERS +=  $${POLICY}/resources.h $${POLICY}/resource-set.h test-resource-set.h
SOURCES +=  $${LIBRESOURCEQT}/src/resource.cpp $${LIBRESOURCEQT}/src/resources.cpp \
            $${LIBRESOURCEQT}/src/resource-set.cpp test-resource-set.cpp

OBJECTS_DIR = build
MOC_DIR = build

CONFIG  += qt qtestlib debug warn_on
QT -= gui

# Install directives
INSTALLBASE    = /usr
target.path    = $${INSTALLBASE}/lib/libresourceqt-tests/
INSTALLS       = target
