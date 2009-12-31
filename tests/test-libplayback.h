#ifndef TEST_RESOURCE_FACTORY_H
#define TEST_RESOURCE_FACTORY_H

#include <QtTest/QTest>
#include "resource-factory.h"

class TestLibplayback: public QObject
{
   Q_OBJECT
private:
   ResourceLibrary *resourceLibrary;
   ResourceFactory *resourceFactory;
public:
   TestLibplayback();
   ~TestLibplayback();
private slots:
   void initTestCase();

   void testCreateResource();
};

#endif
