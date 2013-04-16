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
equals(QT_MAJOR_VERSION, 4): TARGET = resourceqt
equals(QT_MAJOR_VERSION, 5): TARGET = resourceqt5
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
LIBS += $$(DBUSQEVENTLOOPLIB)

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
equals(QT_MAJOR_VERSION, 4): headers.path   = $${INSTALLBASE}/include/resource/qt4/policy
equals(QT_MAJOR_VERSION, 5): headers.path   = $${INSTALLBASE}/include/resource/qt5/policy
equals(QT_MAJOR_VERSION, 4): pc.files       = libresourceqt1.pc
equals(QT_MAJOR_VERSION, 5): pc.files       = libresourceqt5.pc
pc.path        = $${INSTALLBASE}/lib/pkgconfig

man.files      = docs/man
man.path       = $${INSTALLBASE}/share
htmldoc.files   = docs/html
equals(QT_MAJOR_VERSION, 4): htmldoc.path   = $${INSTALLBASE}/share/doc/libresourceqt
equals(QT_MAJOR_VERSION, 5): htmldoc.path   = $${INSTALLBASE}/share/doc/libresourceqt5
xmldoc.files    = docs/xml
equals(QT_MAJOR_VERSION, 4): xmldoc.path    = $${INSTALLBASE}/share/doc/libresourceqt
equals(QT_MAJOR_VERSION, 5): xmldoc.path    = $${INSTALLBASE}/share/doc/libresourceqt5

INSTALLS       = target headers pc man htmldoc xmldoc

