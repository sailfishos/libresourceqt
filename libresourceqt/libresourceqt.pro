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
TEMPLATE = lib
TARGET = resourceqt5
DESTDIR = build
DEPENDPATH += $${POLICY} src
INCLUDEPATH += $${PUBLIC_INCLUDE} $${LIBDBUSQEVENTLOOP} src

# Input
PUBLIC_HEADERS = $${PUBLIC_INCLUDE}/policy/*.h

HEADERS += $${PUBLIC_HEADERS} src/resource-engine.h

SOURCES += src/resource.cpp \
           src/resource-set.cpp \
           src/resource-engine.cpp \
           src/resources.cpp \
           src/audio-resource.cpp

QMAKE_CXXFLAGS += -Wall
LIBS += $${DBUSQEVENTLOOPLIB}

OBJECTS_DIR = build
MOC_DIR = moc

CONFIG  += qt link_pkgconfig dll
QT = core dbus
PKGCONFIG += dbus-1 libresource

# dox.commands   = doxygen Doxyfile
# QMAKE_EXTRA_TARGETS += dox
# PRE_TARGETDEPS += dox

# Install directives
headers.files  = $${PUBLIC_HEADERS}
INSTALLBASE    = /usr
target.path    = $${INSTALLBASE}/lib
headers.path   = $${INSTALLBASE}/include/resource/qt5/policy
pc.files       = libresourceqt5.pc
pc.path        = $${INSTALLBASE}/lib/pkgconfig

man.files      = docs/man
man.path       = $${INSTALLBASE}/share
htmldoc.files   = docs/html
htmldoc.path   = $${INSTALLBASE}/share/doc/libresourceqt5
xmldoc.files    = docs/xml
xmldoc.path    = $${INSTALLBASE}/share/doc/libresourceqt5

INSTALLS       = target headers pc man htmldoc xmldoc

