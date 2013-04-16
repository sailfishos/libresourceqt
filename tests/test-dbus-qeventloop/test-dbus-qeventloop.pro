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

#-------------------------------------------------
#
# Project created by QtCreator 2010-01-18T16:28:34
#
#-------------------------------------------------

include(../../common.pri)

TEMPLATE = app
TARGET = test-dbus-qeventloop
MOC_DIR = .moc
OBJECTS_DIR = .obj
DEPENDPATH += .
QT = core testlib
CONFIG += console link_pkgconfig
CONFIG -= app_bundle
PKGCONFIG += dbus-1

INCLUDEPATH += ../../libdbus-qeventloop
QMAKE_CXXFLAGS += -Wall
LIBS += $${DBUSQEVENTLOOPLIB}

# Input 
SOURCES += test-dbus-qeventloop.cpp 
HEADERS = ../../libdbus-qeventloop/dbusconnectioneventloop.h

QMAKE_DISTCLEAN += -r .moc .obj

# Install options
target.path = /usr/lib/$${TESTSTARGETDIR}/
INSTALLS    = target
