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

include(../common.pri)
TARGET = dbus-qeventloop-qt5
TEMPLATE    = lib
DESTDIR     = build
MOC_DIR     = .moc
OBJECTS_DIR = .obj

DEFINES    += LIBDBUSQEVENTLOOP_LIBRARY
SOURCES    += dbusconnectioneventloop.cpp

HEADERS    += dbusconnectioneventloop.h

QT          = core
CONFIG     += qt link_pkgconfig dll create_pc create_prl no_install_prl
PKGCONFIG  += dbus-1
DEFINES    += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT QT_NO_DEBUG_STREAM

# Install directives
headers.files  = $$HEADERS
INSTALLBASE    = /usr
target.path    = $$INSTALLBASE/lib
headers.path   = $$INSTALLBASE/include

QMAKE_PKGCONFIG_NAME = libdbus-qeventloop5
QMAKE_PKGCONFIG_DESCRIPTION = D-Bus - QEventloop binding
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $$headers.path
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_REQUIRES = dbus-1
QMAKE_PKGCONFIG_VERSION = $$VERSION
QMAKE_PKGCONFIG_FILE = libdbus-qeventloop5

INSTALLS       = target headers

