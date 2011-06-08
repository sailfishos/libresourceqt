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

CONFIG += ordered
TEMPLATE = subdirs

SUBDIRS = libdbus-qeventloop  \
          libresourceqt	      \
          libmediaoverridesqt \
          resourceqt-client   \
          mediaoverrider      \
          tests               \
          demo/player

dist.commands   = ./makedist.sh
FORCE.commands = $(CHK_DIR_EXISTS) libresourceqt/docs/man || mkdir -p libresourceqt/docs/man ;\
                 $(CHK_DIR_EXISTS) libresourceqt/docs/xml || mkdir -p libresourceqt/docs/xml ;\
                 $(CHK_DIR_EXISTS) libresourceqt/docs/html || mkdir -p libresourceqt/docs/html

QMAKE_EXTRA_TARGETS += dist FORCE

