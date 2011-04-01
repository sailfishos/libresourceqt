include(../../common.pri)
TEMPLATE = app

CONFIG += qt link_pkgconfig debug
QT += core
PKGCONFIG += meegotouch 
PKGCONFIG += gstreamer-0.10
PKGCONFIG += gstreamer-interfaces-0.10

DEPENDPATH += $${POLICY} $${BASE}/src .
INCLUDEPATH += $${LIBRESOURCEQT}/src $${LIBRESOURCEQT}/include $${LIBRESOURCEINC} $${LIBDBUSQEVENTLOOP}               
LIBS += -L$${LIBDBUSQEVENTLOOP}/build -L$${LIBRESOURCEQT}/build -ldbus-qeventloop -lresourceqt
    
SOURCES += main.cpp playerpage.cpp playerwidget.cpp streamer.cpp
HEADERS += playerpage.h playerwidget.h streamer.h

# Install options
target.path         = /usr/lib/libresourceqt-demo/
playerdesktop.path  = /usr/share/applications/
playerdesktop.files = policy-player.desktop
INSTALLS            = target playerdesktop
