include(../common.pri)

TEMPLATE = app
TARGET = mediaoverrider
MOC_DIR = moc
OBJECTS_DIR = build
DEPENDPATH += .
INCLUDEPATH += $${LIBMEDIAOVERRIDESQT}
CONFIG += qt meegotouch
QT += dbus

LIBS += -L$${LIBMEDIAOVERRIDESQT}/build -lmediaoverridesqt

# Input
HEADERS += mediaoverrider.h
SOURCES += main.cpp mediaoverrider.cpp

QMAKE_DISTCLEAN += -r moc build

# Install options

target.path = /usr/bin/

desktop.path  = /usr/share/applications/
desktop.files = mediaoverrider.desktop

INSTALLS    = target desktop

