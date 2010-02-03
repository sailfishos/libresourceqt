#-------------------------------------------------
#
# Project created by QtCreator 2010-01-18T16:28:34
#
#-------------------------------------------------

TEMPLATE = app
TARGET = test-dbus-qeventloop
MOC_DIR = .moc
OBJECTS_DIR = .obj
DEPENDPATH += .
QT = testlib core
CONFIG += console link_pkgconfig
CONFIG -= app_bundle
PKGCONFIG += dbus-1

INCLUDEPATH += ../../libdbus-qeventloop
QMAKE_CXXFLAGS += -Wall
LIBS += -L../../libdbus-qeventloop/build -ldbus-qeventloop

# Input 
SOURCES += test-dbus-qeventloop.cpp 

QMAKE_DISTCLEAN += -r .moc .obj

# Install options
target.path = /usr/lib/libresourceqt-tests/
INSTALLS    = target
