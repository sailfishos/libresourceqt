include(../../common.pri)
TEMPLATE = app
TARGET = test-security-with-aegis-token
QT = testlib core
CONFIG -= app_bundle
DEPENDPATH += $${POLICY} $${BASE}/src .
INCLUDEPATH += $${LIBRESOURCEQT}/src $${LIBRESOURCEQT}/include $${LIBRESOURCEINC} $${LIBDBUSQEVENTLOOP}

# Input 
SOURCES += test-security.cpp 

LIBS += -L$${LIBDBUSQEVENTLOOP}/build -L$${LIBRESOURCEQT}/build -ldbus-qeventloop -lresourceqt

# Install options
target.path   = /usr/lib/libresourceqt-tests/
#target.files  = test-security-with-aegis-token 

INSTALLS      = target
