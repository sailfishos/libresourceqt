#include "test-libplayback.h"

using namespace ResourceTypes;

TestLibplayback::TestLibplayback()
{
    resourceFactory=NULL;
    resource=NULL;
    loop=NULL;
    timer=NULL;
}

TestLibplayback::~TestLibplayback()
{
}

void TestLibplayback::init()
{
    loop = new QEventLoop(this);
    timer = new QTimer(this);
    resourceFactory = new ResourceFactory(this);
    QVERIFY(resourceFactory != NULL);
    resource = resourceFactory->createResource(MediaClass, AudioResource|VideoResource);
    QVERIFY(resource != NULL);

    timer->setInterval(3000);
    timer->setSingleShot(true);
}

void TestLibplayback::testConnectToServer()
{
    connect(timer, SIGNAL(timeout()), loop, SLOT(quit()));

    qDebug("connecting to server...");
    resource->connectToServer();

    loop->exec();
}

void TestLibplayback::timeout()
{
    qDebug("Timeout!");
    loop->exit(0);
}

QTEST_MAIN(TestLibplayback)
