##############################################################################
#  This file is part of libresourceqt                                        #
#                                                                            #
#  Copyright (C) 2011 Nokia Corporation.                                     #
#                                                                            #
#  This library is free software; you can redistribute                       #
#  it and/or modify it under the terms of the GNU Lesser General Public      #
#  License as published by the Free Software Foundation                      #
#  version 2.1 of the License.                                               #
#                                                                            #
#  This library is distributed in the hope that it will be useful,           #
#  but WITHOUT ANY WARRANTY; without even the implied warranty of            #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU          #
#  Lesser General Public License for more details.                           #
#                                                                            #
#  You should have received a copy of the GNU Lesser General Public          #
#  License along with this library; if not, write to the Free Software       #
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  #
#  USA.                                                                      #
##############################################################################

CONFIG  += ordered 
TEMPLATE = subdirs

SUBDIRS = test-dbus-qeventloop              \
          test-dbus-pong                    \
          test-audio-resource               \
          test-video-resource               \
          test-resource                     \
          test-resource-set                 \
          test-init-and-connect             \
          benchmark-resource-set            \
          test-acquire                      \
          test-update                       \
          test-auto-release                 \
          test-always-reply                 \
          test-looping                      \
          test-released-by-manager

# Install options
testsxml.path    = /usr/share/libresourceqt-tests/
testsxml.files   = tests.xml
testrunner.path  = /usr/lib/libresourceqt-tests/
testrunner.files = test-dbus-qeventloop-runner.sh
dbusconf.path    = /etc/dbus-1/system.d/
dbusconf.files   = test-dbus-qeventloop.conf
INSTALLS         = testsxml testrunner dbusconf
