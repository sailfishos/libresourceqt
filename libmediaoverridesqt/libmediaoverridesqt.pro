include(../common.pri)
TEMPLATE = lib
TARGET = mediaoverridesqt$${QT_MAJOR_VERSION}
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
INSTALLBASE    = $$[QT_INSTALL_PREFIX]
target.path    = $$[QT_INSTALL_LIBS]
headers.path   = $${INSTALLBASE}/include/resource/qt$${QT_MAJOR_VERSION}/policy

QMAKE_PKGCONFIG_NAME = libmediaoverridesqt$${QT_MAJOR_VERSION}
QMAKE_PKGCONFIG_DESCRIPTION = Maemo playback manager Qt API
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $${INSTALLBASE}/include/resource/qt$${QT_MAJOR_VERSION}
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_REQUIRES = Qt$${QT_MAJOR_VERSION}DBus
QMAKE_PKGCONFIG_VERSION = $$VERSION
QMAKE_PKGCONFIG_FILE = libmediaoverridesqt$${QT_MAJOR_VERSION}

INSTALLS       = target headers

