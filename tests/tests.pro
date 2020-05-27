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

TEMPLATE = subdirs

SUBDIRS = test-audio-resource               \
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
include(test_common.pri)
unix{
    testsxml.path = /usr/share/libresourceqt-qt5-tests/
    testsxml.files    = tests.xml
    testsxml.target   = tests.xml
    testsxml.CONFIG   = no_check_exist
    testsxml.commands = sed \'s%@PATH@%$$[QT_INSTALL_LIBS]/$${TESTSTARGETDIR}%\' $$PWD/tests.xml.in > $$PWD/tests.xml

    QMAKE_DISTCLEAN += tests.xml
}

INSTALLS = testsxml
