include(../common.pri)
TEMPLATE = lib
TARGET = mediaoverridesqt
DESTDIR = build
DEPENDPATH += .
INCLUDEPATH += .

# Input
PUBLIC_HEADERS = override.h

HEADERS += $${PUBLIC_HEADERS}

SOURCES += override.cpp

QMAKE_CXXFLAGS += -Wall
LIBS += -L$${LIBDBUSQEVENTLOOP}/build -ldbus-qeventloop

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
headers.path   = $${INSTALLBASE}/include/resource/qt4/policy
pc.files       = libmediaoverridesqt1.pc
pc.path        = $${INSTALLBASE}/lib/pkgconfig

INSTALLS       = target headers pc

