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

include(../test_common.pri)
TEMPLATE = app
TARGET = test-video-resource
DESTDIR = build

HEADERS += test-video-resource.h

SOURCES += test-video-resource.cpp

OBJECTS_DIR = build
MOC_DIR = build

QMAKE_CXXFLAGS += -Wall

CONFIG  += qt debug warn_on
QT += testlib
QT -= gui

target.path    = $$[QT_INSTALL_LIBS]/$${TESTSTARGETDIR}/
INSTALLS       = target
