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

include(../common.pri)
TEMPLATE = lib
TARGET = resourceqt
DESTDIR = build
DEPENDPATH += $${POLICY} src
INCLUDEPATH += $${LIBRESOURCEINC} $${LIBDBUSQEVENTLOOP} src

# Input
PUBLIC_HEADERS = $${POLICY}/resource.h \
                 $${POLICY}/resource-set.h \
                 $${POLICY}/resources.h \
                 $${POLICY}/audio-resource.h

HEADERS += $${PUBLIC_HEADERS} src/resource-engine.h

SOURCES += src/resource.cpp \
           src/resource-set.cpp \
           src/resource-engine.cpp \
           src/resources.cpp \
           src/audio-resource.cpp

QMAKE_CXXFLAGS += -Wall
LIBS += -L$${LIBDBUSQEVENTLOOP}/build -ldbus-qeventloop

OBJECTS_DIR = build
MOC_DIR = moc

CONFIG  += qt link_pkgconfig dll
QT = core
PKGCONFIG += dbus-1 libresource0

# Install directives
headers.files  = $${PUBLIC_HEADERS}
INSTALLBASE    = /usr
target.path    = $${INSTALLBASE}/lib
headers.path   = $${INSTALLBASE}/include/resource/qt4/policy
pc.files       = libresourceqt1.pc
pc.path        = $${INSTALLBASE}/lib/pkgconfig

INSTALLS       = target headers pc
	
