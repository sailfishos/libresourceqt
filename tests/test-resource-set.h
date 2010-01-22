#ifndef TEST_RESOURCE_SET_H
#define TEST_RESOURCE_SET_H

#include <QtTest/QTest>
#include "resource-set.h"

using namespace ResourcePolicy;

class TestResourceSet: public QObject
{
   Q_OBJECT
private:
   ResourceSet *resourceSet;
   Resource audioPlayback;
   Resource videoPlayback;
   Resource videoRecorder;
   Resource audioRecorder;

public:
   TestResourceSet();
   ~TestResourceSet();

   
private slots:

   void init();
   void cleanup();

   void testIdentifier();

   void testAddResource();
   void testAddResources();
   void testSetResources();

   void testContainsSingle();
   void testDoesNotContainSingle();
   void testContainsSet();
};

#endif
