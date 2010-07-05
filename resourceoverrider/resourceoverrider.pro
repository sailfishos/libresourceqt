include(../common.pri)

TEMPLATE = app
TARGET = resourceoverrider
MOC_DIR = moc
OBJECTS_DIR = build
DEPENDPATH += .
INCLUDEPATH += $${LIBRESOURCEOVERRIDESQT}
CONFIG += qt meegotouch
QT += dbus

LIBS += -L$${LIBRESOURCEOVERRIDESQT}/build -lresource-overridesqt

# Input
HEADERS += resourceoverrider.h
SOURCES += main.cpp resourceoverrider.cpp

QMAKE_DISTCLEAN += -r moc build

# Install options

target.path = /usr/bin/

desktop.path  = /usr/share/applications/
desktop.files = resourceoverrider.desktop

INSTALLS    = target desktop

