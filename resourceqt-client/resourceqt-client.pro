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

TEMPLATE     = app
TARGET = resourceqt$${QT_MAJOR_VERSION}-client
MOC_DIR      = .moc
OBJECTS_DIR  = .obj
DEPENDPATH  += .
QT           = core dbus
CONFIG      += console
CONFIG      -= app_bundle

QMAKE_CXXFLAGS += -Wall
QMAKE_CFLAGS += -std=c99 -D_POSIX_C_SOURCE=199309L
INCLUDEPATH += $${PUBLIC_INCLUDE}
LIBS += $${DBUSQEVENTLOOPLIB} $${RESOURCEQTLIB} -lrt

# Input 
HEADERS     = client.h commandlineparser.h time-stat.h
SOURCES    += resourceqt-client.cpp commandlineparser.cpp client.cpp time-stat.c

QMAKE_DISTCLEAN += -r .moc .obj
#QMAKE_LFLAGS += -rpath-link=$${LIBRESOURCEQT}/build

# Install options
target.path = /usr/bin/
INSTALLS    = target
