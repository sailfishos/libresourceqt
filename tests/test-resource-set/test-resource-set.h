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

    AudioResource *audioResource;
    AudioRecorderResource *audioRecorderResource;
    Resource *videoResource;
    Resource *videoRecorderResource;
    Resource *vibraResource;
    Resource *ledsResource;
    Resource *backlightResource;
    Resource *systemButtonResource;
    Resource *lockButtonResource;
    Resource *scaleButtonResource;
    Resource *snapButtonResource;
    Resource *lensCoverResource;

    Resource * resourceFromType(ResourceType type);
public:
   TestResourceSet();
   ~TestResourceSet();

   
private slots:

   void init();
   void cleanup();

   void testIdentifier();

   void testAddResource();
   void testAddResources();
   void testDelResource();

   void testContainsSingle();
   void testDoesNotContainSingle();
   void testContainsSet();
};

#endif
