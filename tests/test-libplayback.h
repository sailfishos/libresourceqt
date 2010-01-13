#ifndef TEST_RESOURCE_FACTORY_H
#define TEST_RESOURCE_FACTORY_H

#include <QtTest/QTest>
#include <QEventLoop>
#include <QTimer>
#include "resource-factory.h"
#include "resource.h"

class TestLibplayback: public QObject
{
   Q_OBJECT
private:
   ResourceFactory *resourceFactory;
   Resource *resource;
   QEventLoop *loop;
   QTimer *timer;
public:
   TestLibplayback();
   ~TestLibplayback();
public slots:
   void timeout();
private slots:
   void init();

   void testConnectToServer();
};

#endif
