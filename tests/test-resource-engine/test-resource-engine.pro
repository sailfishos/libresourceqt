include(../../common.pri)
TEMPLATE = app
TARGET = test-resource-engine
DESTDIR = build
DEPENDPATH += $${POLICY} $${LIBRESOURCEQT}/src .
INCLUDEPATH += $${LIBRESOURCEQT}/src $${LIBRESOURCEINC} $${LIBDBUSQEVENTLOOP} /usr/include/resource

# Input
HEADERS +=  $${POLICY}/resource.h \
            $${POLICY}/resources.h \
            $${POLICY}/resource-set.h \
            $${LIBRESOURCEQT}/src/resource-engine.h \
            test-resource-engine.h

SOURCES +=  $${LIBRESOURCEQT}/src/resource.cpp \
            $${LIBRESOURCEQT}/src/resources.cpp \
            $${LIBRESOURCEQT}/src/resource-set.cpp \
            $${LIBRESOURCEQT}/src/resource-engine.cpp \
            test-resource-engine.cpp

OBJECTS_DIR = build
MOC_DIR = build
QMAKE_CXXFLAGS += -Wall
LIBS += -L$${LIBDBUSQEVENTLOOP}/build -ldbus-qeventloop

CONFIG  += qt qtestlib debug warn_on link_pkgconfig
QT -= gui
PKGCONFIG += dbus-1

# Install directives
INSTALLBASE    = /usr
target.path    = $${INSTALLBASE}/lib/libresourceqt-tests/
INSTALLS       = target

