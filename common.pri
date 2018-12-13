LIBDBUSQEVENTLOOP = $${PWD}/libdbus-qeventloop
LIBRESOURCEQT = $${PWD}/libresourceqt
LIBMEDIAOVERRIDESQT = $${PWD}/libmediaoverridesqt

RESOURCEQTLIB = -L$${LIBRESOURCEQT}/build -lresourceqt5
DBUSQEVENTLOOPLIB = -L$${LIBDBUSQEVENTLOOP}/build -ldbus-qeventloop-qt5

PUBLIC_INCLUDE = $${LIBRESOURCEQT}/include/

