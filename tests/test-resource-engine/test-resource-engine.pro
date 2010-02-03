BASE = ../../libresourceqt
TEMPLATE = app
TARGET = test-resource-engine
DESTDIR = build
DEPENDPATH += $${BASE}/include $${BASE}/src .
INCLUDEPATH += $${BASE}/src $${BASE}/include $${BASE}/../libresource/src

# Input
HEADERS +=  $${BASE}/include/resource.h \
            $${BASE}/include/resource-set.h \
            $${BASE}/src/resource-engine.h \
            test-resource-engine.h

SOURCES +=  $${BASE}/src/resource.cpp \
            $${BASE}/src/resource-set.cpp \
            $${BASE}/src/resource-engine.cpp \
            test-resource-engine.cpp

OBJECTS_DIR = build
MOC_DIR = build

CONFIG  += qt qtestlib debug warn_on link_pkgconfig
QT -= gui
PKGCONFIG += dbus-1

# Install directives
INSTALLBASE    = /usr
target.path    = $${INSTALLBASE}/lib/libresourceqt-tests/
INSTALLS       = target
