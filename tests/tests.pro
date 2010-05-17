#####################################################################
# Tests projectfile
#####################################################################

CONFIG  += ordered 
TEMPLATE = subdirs

SUBDIRS = test-dbus-qeventloop	\
          test-dbus-pong		\
          test-resource			\
          test-resource-set     \
          test-resource-engine	\
          test-performance

# Install options
testsxml.path    = /usr/share/libresourceqt-tests/
testsxml.files   = tests.xml
testrunner.path  = /usr/lib/libresourceqt-tests/
testrunner.files = test-dbus-qeventloop-runner.sh
dbusconf.path    = /etc/dbus-1/system.d/
dbusconf.files   = test-dbus-qeventloop.conf
INSTALLS         = testsxml testrunner dbusconf
