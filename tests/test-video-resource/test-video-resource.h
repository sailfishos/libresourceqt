/*************************************************************************
This file is part of libresourceqt

Copyright (C) 2011 Nokia Corporation.

This library is free software; you can redistribute
it and/or modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation
version 2.1 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
USA.
*************************************************************************/

#ifndef TESTVIDEORESOURCE_H
#define TESTVIDEORESOURCE_H

#include <QtTest/QTest>
#include <QObject>
#include <policy/resource.h>
#include <policy/resources.h>

using namespace ResourcePolicy;

class TestVideoResource: public QObject
{
    Q_OBJECT
private:
    VideoResource *videoResource;

public:
    TestVideoResource();
    ~TestVideoResource();

private slots:

    void testConstruct1();
    void testConstruct2();
    void testSetProcessId();
};

#endif // TESTVIDEORESOURCE_H
