BASE = ..
TEMPLATE = app
TARGET = test-resource-set
DESTDIR = build
DEPENDPATH += $${BASE}/include $${BASE}/src .
INCLUDEPATH += $${BASE}/src $${BASE}/include

# Input
HEADERS +=  $${BASE}/include/resources.h $${BASE}/include/resource-set.h test-resource-set.h
SOURCES +=  $${BASE}/src/resource.cpp $${BASE}/src/resources.cpp $${BASE}/src/resource-set.cpp test-resource-set.cpp

OBJECTS_DIR = build
MOC_DIR = build

CONFIG  += qt qtestlib debug warn_on
QT -= gui

# Install directives
INSTALLBASE    = /usr
target.path    = $$INSTALLBASE/share/libresourceqt/tests
INSTALLS       = target
