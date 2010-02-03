BASE = ../../libresourceqt
TEMPLATE = app
TARGET = test-resource
DESTDIR = build
DEPENDPATH += $${BASE}/include $${BASE}/src .
INCLUDEPATH += $${BASE}/src $${BASE}/include

# Input
HEADERS +=  $${BASE}/include/resource.h $${BASE}/include/resources.h test-resource.h
SOURCES +=  $${BASE}/src/resource.cpp $${BASE}/src/resources.cpp test-resource.cpp

OBJECTS_DIR = build
MOC_DIR = build

CONFIG  += qt qtestlib debug warn_on
QT -= gui

# Install directives
INSTALLBASE    = /usr
target.path    = $$INSTALLBASE/lib/libresourceqt-tests/
INSTALLS       = target
