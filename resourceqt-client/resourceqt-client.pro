include(../common.pri)

TEMPLATE     = app
TARGET       = resourceqt-client
MOC_DIR      = .moc
OBJECTS_DIR  = .obj
DEPENDPATH  += .
QT           = core
CONFIG      += console
CONFIG      -= app_bundle

INCLUDEPATH += $${LIBRESOURCEINC}
QMAKE_CXXFLAGS += -Wall
QMAKE_CFLAGS += -std=c99 -D_POSIX_C_SOURCE=199309L
LIBS += -L$${LIBRESOURCEQT}/build -lresourceqt -L$${LIBDBUSQEVENTLOOP}/build -ldbus-qeventloop -lm -lrt

# Input 
HEADERS     = client.h commandlineparser.h time-stat.h
SOURCES    += resourceqt-client.cpp commandlineparser.cpp client.cpp time-stat.c

QMAKE_DISTCLEAN += -r .moc .obj

# Install options
target.path = /usr/bin/
INSTALLS    = target
