/*************************************************************************
This file is part of libresourceqt

Copyright (C) 2010 Nokia Corporation.

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
#include "test-init.h"
#include <time.h>


using namespace ResourcePolicy;

static struct timespec startTime;

long int stop_timer(void);
int      start_timer(void);


void TestInitAndConnect::init()
{
     theCase.init();
}


void Case::init()
{
    resSetWithInit           = new ResourcePolicy::ResourceSet("player");
    resSetWithoutInit        = new ResourcePolicy::ResourceSet("player");
    videoResourceWithInit    = new ResourcePolicy::VideoResource;
    videoResourceWithoutInit = new ResourcePolicy::VideoResource;
    //Add the resources.
    resSetWithInit->addResourceObject(videoResourceWithInit);
    resSetWithoutInit->addResourceObject(videoResourceWithoutInit);
}


void TestInitAndConnect::testTheCase()
{
    theCase.initAndConnect();
}


void Case::initAndConnect()
{
    resSetWithInit->initAndConnect();
    QObject::connect(resSetWithInit, SIGNAL(managerIsUp()), &loop, SLOT(quit()));
    loop.exec();
    connectedWithInitHandler();
}


void Case::connectedWithInitHandler()
{
    QVERIFY(resSetWithInit->isConnectedToManager() == true);
    QObject::connect(resSetWithInit, SIGNAL(resourcesGranted(QList<ResourcePolicy::ResourceType>)),
                     &loop,          SLOT(grantedQuit(QList<ResourcePolicy::ResourceType>)));
    startTimer();
    resSetWithInit->acquire();
    loop.exec();
    grantedWithInitHandler();
}


void Case::grantedWithInitHandler()
{
    QVERIFY( videoResourceWithInit->isGranted() == true );
    acquireTimeWithInit = stopTimer(std::string("acquire() with explicit initAndConnect()"));
    QObject::connect(resSetWithInit, SIGNAL(resourcesReleased()), &loop, SLOT(quit()));
    resSetWithInit->release();
    loop.exec();
    releasedWithInitHandler();
}


void Case::releasedWithInitHandler()
{
    QVERIFY( videoResourceWithInit->isGranted() == false );
    resSetWithInit->disconnect();   
    QObject::connect(resSetWithoutInit, SIGNAL(resourcesGranted(QList<ResourcePolicy::ResourceType>)),
                     &loop,             SLOT(grantedQuit(QList<ResourcePolicy::ResourceType>)));
    startTimer();
    resSetWithoutInit->acquire();
    loop.exec();
    grantedWithoutInitHandler();
}


void Case::grantedWithoutInitHandler()
{
    acquireTimeWithoutInit = stopTimer(std::string("acquire() without explicit call to initAndConnect()"));
    QVERIFY( videoResourceWithoutInit->isGranted() == true);

    if (acquireTimeWithoutInit < acquireTimeWithInit)
        QWARN("Acquire time without initAndConnect() shorter.");

    QObject::connect(resSetWithoutInit, SIGNAL(resourcesReleased()), &loop, SLOT(quit()));
    resSetWithoutInit->release();
    loop.exec();
    releasedWithoutInitHandler();
}


void Case::releasedWithoutInitHandler()
{
    QVERIFY( videoResourceWithoutInit->isGranted() == false );
    resSetWithoutInit->disconnect();
}


void Case::startTimer(void) const
{
    start_timer();
}


long int Case::stopTimer(const std::string& op) const
{
        long int ms = stop_timer();
        if (ms > 0)
        {
            qDebug("Operation: %s %ld ms", op.c_str(), ms) ;
        }
        return ms;
}


int start_timer(void)
{
    int r;
    r = clock_gettime(CLOCK_REALTIME, &startTime);

    if (r == 0)
        return 1;
    else
        return 0;
}


long int stop_timer(void)
{
    struct timespec end_time;
    int             r;
    long int        milliseconds = 0L;
    r = clock_gettime(CLOCK_REALTIME, &end_time);

    if (r == 0)
    {
        struct timespec temp;
        if (end_time.tv_nsec < startTime.tv_nsec)
        {
            temp.tv_sec  = end_time.tv_sec - startTime.tv_sec - 1;
            temp.tv_nsec = 1000000000 + end_time.tv_nsec - startTime.tv_nsec;
        }
        else
        {
            temp.tv_sec  = end_time.tv_sec  - startTime.tv_sec;
            temp.tv_nsec = end_time.tv_nsec - startTime.tv_nsec;
        }
        milliseconds = (1000 * temp.tv_sec) + (temp.tv_nsec / 1000000);

        if (temp.tv_nsec % 1000000 > 500000)
        {
            ++milliseconds;
        }
    }
    startTime.tv_sec  = 0;
    startTime.tv_nsec = 0;

    return milliseconds;
}



int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    TestInitAndConnect t;
    return QTest::qExec(&t, argc, argv);
}

