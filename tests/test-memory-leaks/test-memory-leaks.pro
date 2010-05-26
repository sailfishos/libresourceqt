include(../../common.pri)
TEMPLATE = app
TARGET = test-memory-leaks
DESTDIR = build
DEPENDPATH += $${POLICY} $${BASE}/src .
INCLUDEPATH += $${LIBRESOURCEQT}/src $${LIBRESOURCEQT}/include $${LIBRESOURCEINC} $${LIBDBUSQEVENTLOOP}

# Input
HEADERS += test-memory-leaks.h
SOURCES += test-memory-leaks.cpp 

OBJECTS_DIR = build
MOC_DIR = build

LIBS += -L$${LIBDBUSQEVENTLOOP}/build -L$${LIBRESOURCEQT}/build -ldbus-qeventloop -lresourceqt

CONFIG  += qt debug warn_on link_pkgconfig
QT -= gui
PKGCONFIG += dbus-1 libresource0

# Install directives
INSTALLBASE    = /usr
target.path    = $${INSTALLBASE}/lib/libresourceqt-tests/
INSTALLS       = target
