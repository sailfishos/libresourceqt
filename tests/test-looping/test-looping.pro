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
TARGET = test-looping
DESTDIR = build
DEPENDPATH += $${POLICY} $${BASE}/src .
INCLUDEPATH += $${LIBRESOURCEQT}/src $${LIBRESOURCEINC} $${LIBDBUSQEVENTLOOP}

# Silence qDebug
DEFINES += QT_NO_DEBUG_OUTPUT

# Input
HEADERS +=  $${POLICY}/resources.h \
            $${POLICY}/resource-set.h \
            $${POLICY}/audio-resource.h \
            $${LIBRESOURCEQT}/src/resource-engine.h \
            test-looping.h

SOURCES +=  $${LIBRESOURCEQT}/src/resource.cpp \
            $${LIBRESOURCEQT}/src/resources.cpp \
            $${LIBRESOURCEQT}/src/resource-engine.cpp \
            $${LIBRESOURCEQT}/src/audio-resource.cpp \
            $${LIBRESOURCEQT}/src/resource-set.cpp \
            test-looping.cpp

OBJECTS_DIR = build
MOC_DIR = build

QMAKE_CXXFLAGS += -Wall
LIBS += $${DBUSQEVENTLOOPLIB}

CONFIG  += qt debug warn_on link_pkgconfig
QT += testlib
QT -= gui
PKGCONFIG += dbus-1 libresource

# Install directives
INSTALLBASE    = /usr
target.path    = $${INSTALLBASE}/lib/$${TESTSTARGETDIR}/
INSTALLS       = target
