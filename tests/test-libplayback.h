#ifndef TEST_RESOURCE_FACTORY_H
#define TEST_RESOURCE_FACTORY_H

#include <QtTest/QTest>
#include "resource-factory.h"
#include "resource.h"

class TestLibplayback: public QObject
{
   Q_OBJECT
private:
   ResourceFactory *resourceFactory;
   Resource *resource;
public:
   TestLibplayback();
   ~TestLibplayback();
private slots:
   void initTestCase();

   void testConnectToServer();
};

#endif
