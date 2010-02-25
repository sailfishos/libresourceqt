#####################################################################
# Main projectfile
#####################################################################

CONFIG += ordered
TEMPLATE = subdirs

SUBDIRS = libdbus-qeventloop	\
          libresourceqt			\
          resourceqt-client		\
          tests
