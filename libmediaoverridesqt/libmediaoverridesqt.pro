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

CONFIG  += qt dll create_pc create_prl no_install_prl
QT += dbus
PKGCONFIG += dbus-1 libresource

QMAKE_DISTCLEAN += -r moc build

# Install directives
headers.files  = $${PUBLIC_HEADERS}
INSTALLBASE    = /usr
target.path    = $${INSTALLBASE}/lib
headers.path   = $${INSTALLBASE}/include/resource/qt5/policy

QMAKE_PKGCONFIG_NAME = libmediaoverridesqt5
QMAKE_PKGCONFIG_DESCRIPTION = Maemo playback manager Qt API
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $${INSTALLBASE}/include/resource/qt5
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_REQUIRES = Qt5DBus
QMAKE_PKGCONFIG_VERSION = $$VERSION
QMAKE_PKGCONFIG_FILE = libmediaoverridesqt5

INSTALLS       = target headers

