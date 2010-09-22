##############################################################################
#  This file is part of libresourceqt                                        #
#                                                                            #
#  Copyright (C) 2010 Nokia Corporation.                                     #
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
TARGET = test-resource-engine
DESTDIR = build
DEPENDPATH += $${POLICY} $${LIBRESOURCEQT}/src .
INCLUDEPATH += $${LIBRESOURCEQT}/src $${LIBRESOURCEINC} $${LIBDBUSQEVENTLOOP} /usr/include/resource

# Input
HEADERS +=  $${POLICY}/resource.h \
            $${POLICY}/resources.h \
            $${POLICY}/resource-set.h \
            $${LIBRESOURCEQT}/src/resource-engine.h \
            $${POLICY}/audio-resource.h \
            test-resource-engine.h

SOURCES +=  $${LIBRESOURCEQT}/src/resource.cpp \
            $${LIBRESOURCEQT}/src/resources.cpp \
            $${LIBRESOURCEQT}/src/resource-set.cpp \
            $${LIBRESOURCEQT}/src/resource-engine.cpp \
            $${LIBRESOURCEQT}/src/audio-resource.cpp \
            test-resource-engine.cpp

OBJECTS_DIR = build
MOC_DIR = build/moc
QMAKE_CXXFLAGS += -Wall
LIBS += -L$${LIBDBUSQEVENTLOOP}/build -ldbus-qeventloop

CONFIG  += qt qtestlib debug warn_on link_pkgconfig
QT -= gui
PKGCONFIG += dbus-1

# Install directives
INSTALLBASE    = /usr
target.path    = $${INSTALLBASE}/lib/libresourceqt-tests/

wrapper.path  = $${INSTALLBASE}/lib/libresourceqt-tests/
wrapper.files = test-resource-engine-wrapper

INSTALLS       = target wrapper
