#####################################################################
# Main projectfile
#####################################################################

CONFIG += ordered
TEMPLATE = subdirs

SUBDIRS = libdbus-qeventloop	  \
          libresourceqt			  \
          libresource-overridesqt \
          resourceqt-client		  \
          resourceoverrider       \
          tests

