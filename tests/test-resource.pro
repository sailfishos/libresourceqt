BASE = ..
TEMPLATE = app
TARGET = test-resource
DESTDIR = build
DEPENDPATH += $${BASE}/include $${BASE}/src .
INCLUDEPATH += $${BASE}/src $${BASE}/include

# Input
HEADERS +=  $${BASE}/include/resource.h test-resource.h mock-resource-library.h
SOURCES +=  $${BASE}/src/resource.cpp test-resource.cpp mock-resource-library.cpp

OBJECTS_DIR = build
MOC_DIR = build

CONFIG  += qt qtestlib debug warn_on
QT -= gui

# Install directives
INSTALLBASE    = /usr
target.path    = $$INSTALLBASE/share/libresourceqt/tests
INSTALLS       = target
