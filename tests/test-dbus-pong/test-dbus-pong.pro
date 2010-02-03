TEMPLATE = app
TARGET = test-dbus-pong
MOC_DIR = .moc
OBJECTS_DIR = .obj
DEPENDPATH += .
QT = core dbus
CONFIG += console
CONFIG -= app_bundle

QMAKE_CXXFLAGS += -Wall

# Input 
SOURCES += test-dbus-pong.cpp 
HEADERS += pong.h

QMAKE_DISTCLEAN += -r .moc .obj

# Install options
target.path = /usr/lib/libresourceqt-tests/
INSTALLS    = target
