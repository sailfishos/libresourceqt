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
TARGET = resourceqt$${QT_MAJOR_VERSION}
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

CONFIG  += qt link_pkgconfig dll create_pc create_prl no_install_prl
QT = core dbus
PKGCONFIG += dbus-1 libresource

# dox.commands   = doxygen Doxyfile
# QMAKE_EXTRA_TARGETS += dox
# PRE_TARGETDEPS += dox

# Install directives
headers.files  = $${PUBLIC_HEADERS}
INSTALLBASE    = $$[QT_INSTALL_PREFIX]
target.path    = $$[QT_INSTALL_LIBS]
headers.path   = $${INSTALLBASE}/include/resource/qt$${QT_MAJOR_VERSION}/policy

man.files      = docs/man
man.path       = $${INSTALLBASE}/share
htmldoc.files   = docs/html
htmldoc.path   = $${INSTALLBASE}/share/doc/libresourceqt$${QT_MAJOR_VERSION}
xmldoc.files    = docs/xml
xmldoc.path    = $${INSTALLBASE}/share/doc/libresourceqt$${QT_MAJOR_VERSION}

QMAKE_PKGCONFIG_NAME = libresourceqt$${QT_MAJOR_VERSION}
QMAKE_PKGCONFIG_DESCRIPTION = Maemo resource management Qt API
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $${INSTALLBASE}/include/resource/qt$${QT_MAJOR_VERSION}
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_REQUIRES = dbus-1 libdbus-qeventloop$${QT_MAJOR_VERSION} libresource Qt$${QT_MAJOR_VERSION}Core
QMAKE_PKGCONFIG_VERSION = $$VERSION
QMAKE_PKGCONFIG_FILE = libresourceqt$${QT_MAJOR_VERSION}

INSTALLS       = target headers man htmldoc xmldoc

