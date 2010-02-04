#####################################################################
# Tests projectfile
#####################################################################

CONFIG  += ordered 
TEMPLATE = subdirs

SUBDIRS = test-dbus-qeventloop	\
          test-dbus-pong		\
          test-resource			\
#          test-resource-engine	\
          test-resource-set

# Install options
testsxml.path  = /usr/share/libresourceqt-tests/
testsxml.files = tests.xml
INSTALLS       = testsxml
