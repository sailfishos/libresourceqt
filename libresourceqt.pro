#####################################################################
# Main projectfile
#####################################################################

CONFIG += ordered
TEMPLATE = subdirs

SUBDIRS = libdbus-qeventloop  \
          libresourceqt	      \
          libmediaoverridesqt \
          resourceqt-client   \
          mediaoverrider      \
          tests

