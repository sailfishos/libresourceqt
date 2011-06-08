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

include(../../common.pri)
TEMPLATE = app
TARGET = test-memory-leaks
DESTDIR = build
DEPENDPATH += $${POLICY} $${BASE}/src .
INCLUDEPATH += $${LIBRESOURCEQT}/src $${LIBRESOURCEQT}/include $${LIBRESOURCEINC} $${LIBDBUSQEVENTLOOP}

# Input
HEADERS += test-memory-leaks.h
SOURCES += test-memory-leaks.cpp 

OBJECTS_DIR = build
MOC_DIR = build

LIBS += -L$${LIBDBUSQEVENTLOOP}/build -L$${LIBRESOURCEQT}/build -ldbus-qeventloop -lresourceqt

CONFIG  += qt debug warn_on link_pkgconfig
QT -= gui
PKGCONFIG += dbus-1 libresource0

# Install directives
INSTALLBASE    = /usr
target.path    = $${INSTALLBASE}/lib/libresourceqt-tests/
INSTALLS       = target
