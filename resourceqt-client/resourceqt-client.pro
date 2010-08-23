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
LIBS += -L$${LIBRESOURCEQT}/build -lresourceqt -L$${LIBDBUSQEVENTLOOP}/build -ldbus-qeventloop

# Input 
HEADERS     = client.h commandlineparser.h time-stat.h
SOURCES    += resourceqt-client.cpp commandlineparser.cpp client.cpp time-stat.c

QMAKE_DISTCLEAN += -r .moc .obj

# Install options
target.path = /usr/bin/
INSTALLS    = target
