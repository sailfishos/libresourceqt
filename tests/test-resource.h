#ifndef TEST_RESOURCE_H
#define TEST_RESOURCE_H

#include <QtTest/QTest>
#include <QMetaType>
#include "resource.h"

Q_DECLARE_METATYPE(ResourcePolicy::ResourceType)

using namespace ResourcePolicy;

class TestResource: public QObject
{
   Q_OBJECT
private:
   Resource *resource;

public:
   TestResource();
   ~TestResource();

private slots:

   void init();
   void cleanup();

   void testType_data();
   void testType();
   
   void testOptional_data();
   void testOptional();

   void testShared_data();
   void testShared();

   void testIdentifier_data();
   void testIdentifier();

   void testCopy();
   void testCopyConstructor();

   void testEqualsOperator();

   void testHashFunctionWithCopies();
   void testHashFunctionWithDifferentResources();
   void testHashFunctionWithDifferentOptionality();
   void testHashFunctionWithDifferentSharedness();
   void testHashFunctionWithIdentical();

   void testQSetOfResource();
};

#endif
