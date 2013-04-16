LIBDBUSQEVENTLOOP = $${PWD}/libdbus-qeventloop
LIBRESOURCEQT = $${PWD}/libresourceqt
LIBMEDIAOVERRIDESQT = $${PWD}/libmediaoverridesqt

equals(QT_MAJOR_VERSION, 4) {
    LIBRESOURCEINC = $${LIBRESOURCEQT}/include/qt4
    RESOURCEQTLIB = -L$${LIBRESOURCEQT}/build -lresourceqt
    DBUSQEVENTLOOPLIB = -L$${LIBDBUSQEVENTLOOP}/build -ldbus-qeventloop
    TESTSTARGETDIR = libresourceqt-tests
}
equals(QT_MAJOR_VERSION, 5) {
    LIBRESOURCEINC = $${LIBRESOURCEQT}/include/qt5
    RESOURCEQTLIB = -L$${LIBRESOURCEQT}/build -lresourceqt5
    DBUSQEVENTLOOPLIB = -L$${LIBDBUSQEVENTLOOP}/build -ldbus-qeventloop-qt5
    TESTSTARGETDIR = libresourceqt5-tests
}

POLICY = $${LIBRESOURCEINC}/policy

VERSION = 1.0.0

