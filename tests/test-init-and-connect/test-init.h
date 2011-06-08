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

#ifndef TEST_INIT_H
#define TEST_INIT_H

#include <QtTest/QTest>
#include <QMetaType>
#include <policy/resource-set.h>
#include <policy/resources.h>

using namespace ResourcePolicy;


class myEvtLoop: public QEventLoop
{
    Q_OBJECT

public slots:
    void grantedQuit(const QList<ResourcePolicy::ResourceType>&)
    {
        quit();
    }
};


class Case
{
public:
    Case(): acquireTimeWithInit(0), acquireTimeWithoutInit(0){}

    ~Case(){
        delete resSetWithInit;
        delete resSetWithoutInit;
    }
    void init();
    void initAndConnect();

private:
    myEvtLoop    loop;

    long int     acquireTimeWithInit;
    long int     acquireTimeWithoutInit;

    ResourceSet* resSetWithInit;
    ResourceSet* resSetWithoutInit;
    //libresourceqt takes ownership.
    Resource*    videoResourceWithInit;
    Resource*    videoResourceWithoutInit;

    void        startTimer(void) const;
    long int    stopTimer(const std::string& op) const;
    //Wrap these "would be" slots so that they don't get
    //called by the QT's test framework.
    void        connectedWithInitHandler();
    void        grantedWithInitHandler();
    void        releasedWithInitHandler();
    void        grantedWithoutInitHandler();
    void        releasedWithoutInitHandler();
};


class TestInitAndConnect: public QObject
{
    Q_OBJECT

private:
    Case theCase;
private slots:
    void init();
    void testTheCase();
};




#endif

