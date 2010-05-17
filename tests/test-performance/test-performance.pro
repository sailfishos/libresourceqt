#####################################################################
# Tests projectfile
#####################################################################

CONFIG  += ordered 
TEMPLATE = subdirs

SUBDIRS = dbus-spammer test-resourceqt-performance test-resourceqt-performance-collector

# Install options
testsxml.path    = /usr/share/libresourceqt-performance-tests/
testsxml.files   = tests.xml
testrunner.path  = /usr/lib/libresourceqt-performance-tests/
testrunner.files = test-resourceqt-performance-runner.sh test-resourceqt-performance.ini template.html
INSTALLS         = testsxml testrunner
