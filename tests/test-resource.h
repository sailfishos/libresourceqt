#ifndef TEST_RESOURCE_H
#define TEST_RESOURCE_H

#include <QtTest/QTest>
#include "resource.h"
#include "mock-resource-library.h"

class TestResource: public QObject
{
   Q_OBJECT
private:
   ResourceLibrary *resourceLibrary;
   Resource *resource;

   bool isReservable;
public:
   TestResource();
   ~TestResource();

public slots:
   void handleReservable();

private slots:
   void init();

   void testInitializeSucceeds();
   void testInitializeFails();

   void testConnectToServerSucceeds();
   void testConnectToServerFails();

   void testApplicationClass();
   void testResources();

   void testReservable();
};

#endif
