#include "test-resource-set.h"

using namespace ResourcePolicy;

TestResourceSet::TestResourceSet()
{
    audioPlayback.setType(AudioPlaybackResource);
    videoPlayback.setType(VideoPlaybackResource);
    audioRecorder.setType(AudioRecorderResource);
    videoRecorder.setType(VideoRecorderResource);    
}

TestResourceSet::~TestResourceSet()
{
}

void TestResourceSet::init()
{
    resourceSet = new ResourceSet("player");
}

void TestResourceSet::cleanup()
{
    delete resourceSet;
}

void TestResourceSet::testIdentifier()
{
    ResourceSet otherSet("game");

    bool identifiersAreUnique = (resourceSet->id() != otherSet.id());
    QVERIFY(identifiersAreUnique);
    QVERIFY(resourceSet->id() == (quint32)resourceSet);
    QVERIFY(otherSet.id() == (quint32)&otherSet);
}

void TestResourceSet::testAddResource()
{
    Resource resource;
    resource.setType(AudioPlaybackResource);

    resourceSet->addResource(resource);

    QSet<Resource> resourcesInSet = resourceSet->resources();
    QVERIFY(resourcesInSet.contains(resource));
}

void TestResourceSet::testAddResources()
{
    QSet<Resource> resources;

    resources << audioPlayback << videoPlayback;

    resourceSet->addResources(resources);

    QSet<Resource> resourcesInSet = resourceSet->resources();
    QVERIFY(resourcesInSet == resources);
}

void TestResourceSet::testSetResources()
{
    QSet<Resource> resources, newResources;

    newResources << audioPlayback << videoPlayback;
    resources << audioPlayback << videoPlayback << audioRecorder << videoRecorder;
    resourceSet->setResources(resources);
    QSet<Resource> resourcesInSet = resourceSet->resources();

    resourceSet->setResources(newResources);

    QSet<Resource> resourcesInNewSet = resourceSet->resources();
    QVERIFY(resourcesInNewSet == newResources);
}

void TestResourceSet::testContainsSingle()
{
    QSet<Resource> resources;

    resources << audioPlayback << videoPlayback << audioRecorder;
    resourceSet->setResources(resources);
    QSet<Resource> resourcesInSet = resourceSet->resources();
    resourceSet->setResources(resources);

    bool containsVideoPlayback = resourceSet->contains(videoPlayback);
    QVERIFY(containsVideoPlayback);
}

void TestResourceSet::testDoesNotContainSingle()
{
    QSet<Resource> resources;

    resources << audioPlayback << videoPlayback;
    resourceSet->setResources(resources);
    QSet<Resource> resourcesInSet = resourceSet->resources();
    resourceSet->setResources(resources);

    bool containsVideoRecorder = resourceSet->contains(videoRecorder);
    QEXPECT_FAIL("", "This should fail since searched resource should NOT be in set", Continue);
    QVERIFY(containsVideoRecorder);
}


void TestResourceSet::testContainsSet()
{
    QSet<Resource> resources, subset;

    resources << audioPlayback << videoPlayback << audioRecorder << videoRecorder;
    subset << audioPlayback << videoPlayback;

    resourceSet->setResources(resources);
    QSet<Resource> resourcesInSet = resourceSet->resources();
    resourceSet->setResources(resources);

    bool containsSubset = resourceSet->contains(subset);
    QVERIFY(containsSubset);
}
QTEST_MAIN(TestResourceSet)
