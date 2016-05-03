include(../common.pri)
TEMPLATE = lib
TARGET = mediaoverridesqt5
DESTDIR = build
DEPENDPATH += .
INCLUDEPATH += .

# Input
PUBLIC_HEADERS = override.h

HEADERS += $${PUBLIC_HEADERS}

SOURCES += override.cpp

QMAKE_CXXFLAGS += -Wall
LIBS += $$(DBUSQEVENTLOOPLIB)

OBJECTS_DIR = build
MOC_DIR = moc

CONFIG  += qt dll
QT += dbus
PKGCONFIG += dbus-1 libresource

QMAKE_DISTCLEAN += -r moc build

# Install directives
headers.files  = $${PUBLIC_HEADERS}
INSTALLBASE    = /usr
target.path    = $${INSTALLBASE}/lib
headers.path   = $${INSTALLBASE}/include/resource/qt5/policy
pc.files       = libmediaoverridesqt5.pc
pc.path        = $${INSTALLBASE}/lib/pkgconfig

INSTALLS       = target headers pc

