include(../../common.pri)
TEMPLATE = app
TARGET = test-resource-set
DESTDIR = build
DEPENDPATH += $${POLICY} $${BASE}/src .
INCLUDEPATH += $${LIBRESOURCEQT}/src $${LIBRESOURCEINC} $${LIBDBUSQEVENTLOOP}

# Input
HEADERS +=  $${POLICY}/resources.h $${POLICY}/resource-set.h test-resource-set.h \
            $${LIBRESOURCEQT}/src/resource-engine.h

SOURCES +=  $${LIBRESOURCEQT}/src/resource.cpp $${LIBRESOURCEQT}/src/resources.cpp \
            $${LIBRESOURCEQT}/src/resource-engine.cpp \
            $${LIBRESOURCEQT}/src/resource-set.cpp test-resource-set.cpp 

OBJECTS_DIR = build
MOC_DIR = build

QMAKE_CXXFLAGS += -Wall
LIBS += -L$${LIBDBUSQEVENTLOOP}/build -ldbus-qeventloop

CONFIG  += qt qtestlib debug warn_on link_pkgconfig
QT -= gui
PKGCONFIG += dbus-1 libresource0

# Install directives
INSTALLBASE    = /usr
target.path    = $${INSTALLBASE}/lib/libresourceqt-tests/
INSTALLS       = target
