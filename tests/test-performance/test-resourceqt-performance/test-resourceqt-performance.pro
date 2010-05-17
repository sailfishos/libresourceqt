include(../../../common.pri)

TEMPLATE     = app
TARGET       = test-resourceqt-performance
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
SOURCES    += test-resourceqt-performance.cpp client.cpp

QMAKE_DISTCLEAN += -r .moc .obj

# Install options
target.path = /usr/lib/libresourceqt-performance-tests/
INSTALLS    = target
