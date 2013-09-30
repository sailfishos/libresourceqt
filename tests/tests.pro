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
include(../common.pri)
unix{
    equals(QT_MAJOR_VERSION, 4): testsxml.path = /usr/share/libresourceqt-tests/
    equals(QT_MAJOR_VERSION, 5): testsxml.path = /usr/share/libresourceqt-qt5-tests/
    testsxml.files    = tests.xml
    testsxml.target   = tests.xml
    testsxml.CONFIG   = no_check_exist
    testsxml.commands = sed \'s%@PATH@%/usr/lib/$${TESTSTARGETDIR}%\' $$PWD/tests.xml.in > $$PWD/tests.xml

    equals(QT_MAJOR_VERSION, 4): testrunner.path  = /usr/lib/libresourceqt-tests/
    equals(QT_MAJOR_VERSION, 5): testrunner.path  = /usr/lib/libresourceqt-qt5-tests/
    testrunner.files    = test-dbus-qeventloop-runner.sh
    testrunner.target   = test-dbus-qeventloop-runner.sh
    testrunner.CONFIG   = no_check_exist
    testrunner.commands = sed \'s%@PATH@%/usr/lib/$${TESTSTARGETDIR}%\' $$PWD/test-dbus-qeventloop-runner.sh.in > $$PWD/test-dbus-qeventloop-runner.sh

    QMAKE_DISTCLEAN += tests.xml test-dbus-qeventloop-runner.sh
}
dbusconf.path    = /etc/dbus-1/system.d/
dbusconf.files   = test-dbus-qeventloop.conf
INSTALLS         = testsxml testrunner dbusconf
