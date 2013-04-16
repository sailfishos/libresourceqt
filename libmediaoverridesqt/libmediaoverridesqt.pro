include(../common.pri)
TEMPLATE = lib
equals(QT_MAJOR_VERSION, 4): TARGET = mediaoverridesqt
equals(QT_MAJOR_VERSION, 5): TARGET = mediaoverridesqt5
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
equals(QT_MAJOR_VERSION, 4): headers.path   = $${INSTALLBASE}/include/resource/qt4/policy
equals(QT_MAJOR_VERSION, 5): headers.path   = $${INSTALLBASE}/include/resource/qt5/policy
equals(QT_MAJOR_VERSION, 4): pc.files       = libmediaoverridesqt1.pc
equals(QT_MAJOR_VERSION, 5): pc.files       = libmediaoverridesqt5.pc
pc.path        = $${INSTALLBASE}/lib/pkgconfig

INSTALLS       = target headers pc

