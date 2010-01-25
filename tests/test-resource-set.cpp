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

    QList<Resource> resourcesInSet = resourceSet->resources();
    QVERIFY(resourcesInSet.contains(resource));
}

void TestResourceSet::testAddResources()
{
    QList<Resource> resources;

    resources << audioPlayback << videoPlayback;

    resourceSet->addResources(resources);

    QList<Resource> resourcesInSet = resourceSet->resources();
    bool resultContainsAllItems = false;
    for(int i=0; i<resourcesInSet.size(); i++) {
	resultContainsAllItems = resources.contains(resourcesInSet.at(i));
	if(!resultContainsAllItems) {
	    qDebug("resources doesn't contain Resource 0x%02x", resourcesInSet.at(i).type());
	    break;
	}
    }
    QVERIFY(resultContainsAllItems);
    for(int i=0; i<resources.size(); i++) {
	resultContainsAllItems = resourcesInSet.contains(resources.at(i));
	if(!resultContainsAllItems) {
	    break;
	}
    }
    QVERIFY(resultContainsAllItems);
}

void TestResourceSet::testSetResources()
{
    QList<Resource> resources, newResources;

    newResources << audioPlayback << videoPlayback;
    resources << audioPlayback << videoPlayback << audioRecorder << videoRecorder;
    resourceSet->setResources(resources);
    QList<Resource> resourcesInSet = resourceSet->resources();

    resourceSet->setResources(newResources);

    QList<Resource> resourcesInNewSet = resourceSet->resources();
    bool resultContainsAllItems = false;
    for(int i=0; i<resourcesInNewSet.size(); i++) {
	resultContainsAllItems = newResources.contains(resourcesInNewSet.at(i));
	if(!resultContainsAllItems) {
	    qDebug("newResources doesn't contain Resource 0x%02x", resourcesInNewSet.at(i).type());
	    break;
	}
    }
    QVERIFY(resultContainsAllItems);
    for(int i=0; i<newResources.size(); i++) {
	resultContainsAllItems = resourcesInNewSet.contains(newResources.at(i));
	if(!resultContainsAllItems) {
	    qDebug("newResources doesn't contain Resource 0x%02x", resourcesInNewSet.at(i).type());
	    break;
	}
    }
    QVERIFY(resultContainsAllItems);
}

void TestResourceSet::testContainsSingle()
{
    QList<Resource> resources;

    resources << audioPlayback << videoPlayback << audioRecorder;
    resourceSet->setResources(resources);
    QList<Resource> resourcesInSet = resourceSet->resources();
    resourceSet->setResources(resources);

    bool containsVideoPlayback = resourceSet->contains(videoPlayback);
    QVERIFY(containsVideoPlayback);
}

void TestResourceSet::testDoesNotContainSingle()
{
    QList<Resource> resources;

    resources << audioPlayback << videoPlayback;
    resourceSet->setResources(resources);
    QList<Resource> resourcesInSet = resourceSet->resources();
    resourceSet->setResources(resources);

    bool containsVideoRecorder = resourceSet->contains(videoRecorder);
    QEXPECT_FAIL("", "This should fail since searched resource should NOT be in set", Continue);
    QVERIFY(containsVideoRecorder);
}


void TestResourceSet::testContainsSet()
{
    QList<Resource> resources, subset;

    resources << audioPlayback << videoPlayback << audioRecorder << videoRecorder;
    subset << audioPlayback << videoPlayback;

    resourceSet->setResources(resources);
    QList<Resource> resourcesInSet = resourceSet->resources();
    resourceSet->setResources(resources);

    bool containsSubset = resourceSet->contains(subset);
    QVERIFY(containsSubset);
}
QTEST_MAIN(TestResourceSet)
