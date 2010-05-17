TEMPLATE = app
TARGET = dbus-spammer
MOC_DIR = .moc
OBJECTS_DIR = .obj
DEPENDPATH += .
QT = core dbus
CONFIG += console link_pkgconfig
CONFIG -= app_bundle
PKGCONFIG += dbus-1

QMAKE_CXXFLAGS += -Wall

# Input 
SOURCES += dbus-spammer.cpp dbus-services.cpp
HEADERS += dbus-services.h

QMAKE_DISTCLEAN += -r .moc .obj

# Install options
target.path = /usr/lib/libresourceqt-performance-tests/
INSTALLS    = target
