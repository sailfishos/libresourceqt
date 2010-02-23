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
HEADERS     = client.h
SOURCES    += resourceqt-client.cpp client.cpp

QMAKE_DISTCLEAN += -r .moc .obj

# Install options
target.path = /usr/bin/
INSTALLS    = target
