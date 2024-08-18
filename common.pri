LIBDBUSQEVENTLOOP = $${PWD}/libdbus-qeventloop
LIBRESOURCEQT = $${PWD}/libresourceqt
LIBMEDIAOVERRIDESQT = $${PWD}/libmediaoverridesqt

RESOURCEQTLIB = -L$${LIBRESOURCEQT}/build -lresourceqt$${QT_MAJOR_VERSION}
DBUSQEVENTLOOPLIB = -L$${LIBDBUSQEVENTLOOP}/build -ldbus-qeventloop-qt$${QT_MAJOR_VERSION}

PUBLIC_INCLUDE = $${LIBRESOURCEQT}/include/

