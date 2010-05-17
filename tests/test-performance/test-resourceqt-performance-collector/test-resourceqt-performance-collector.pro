TEMPLATE     = app
TARGET       = test-resourceqt-performance-collector
MOC_DIR      = .moc
OBJECTS_DIR  = .obj
DEPENDPATH  += .
QT           = core network
CONFIG      += console
CONFIG      -= app_bundle

INCLUDEPATH += $${LIBRESOURCEINC}
QMAKE_CXXFLAGS += -Wall

# Input 
HEADERS     = ResultsUpload.h
SOURCES    += test-resourceqt-performance-collector.cpp ResultsUpload.cpp

QMAKE_DISTCLEAN += -r .moc .obj

# Install options
target.path = /usr/lib/libresourceqt-performance-tests/
INSTALLS    = target
