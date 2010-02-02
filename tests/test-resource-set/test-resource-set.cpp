#include "test-resource-set.h"

Resource * TestResourceSet::resourceFromType(ResourceType type)
{
    switch(type) {
    case AudioPlaybackType:
        return audioResource;
    case AudioRecorderType:
        return audioRecorderResource;
    case VideoPlaybackType:
        return videoResource;
    case VideoRecorderType:
        return videoRecorderResource;
    case VibraType:
        return vibraResource;
    case LedsType:
        return ledsResource;
    case BacklightType:
        return backlightResource;
    case SystemButtonType:
        return systemButtonResource;
    case LockButtonType:
        return lockButtonResource;
    case ScaleButtonType:
        return scaleButtonResource;
    case SnapButtonType:
        return snapButtonResource;
    case LensCoverType:
        return lensCoverResource;
    default:
        return NULL;
    }
}

using namespace ResourcePolicy;

TestResourceSet::TestResourceSet()
{
    audioResource = new AudioResource;
    audioRecorderResource = new AudioRecorderResource;
    videoResource = new VideoResource;
    videoRecorderResource = new VideoRecorderResource;
    vibraResource = new VibraResource;
    ledsResource = new LedsResource;
    backlightResource = new BacklightResource;
    systemButtonResource = new SystemButtonResource;
    lockButtonResource = new LockButtonResource;
    scaleButtonResource = new ScaleButtonResource;
    snapButtonResource = new SnapButtonResource;
    lensCoverResource = new LensCoverResource;
}

TestResourceSet::~TestResourceSet()
{
    delete audioResource;
    delete audioRecorderResource;
    delete videoResource;
    delete videoRecorderResource;
    delete vibraResource;
    delete ledsResource;
    delete backlightResource;
    delete systemButtonResource;
    delete lockButtonResource;
    delete scaleButtonResource;
    delete snapButtonResource;
    delete lensCoverResource;
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
    for(int i=0;i<NumberOfTypes;i++) {
        ResourceType type = (ResourceType)i;
        Resource *resource = resourceFromType(type);
        resourceSet->addResource(resource);
        bool setContainsGivenResource = resourceSet->contains(type);
        QVERIFY(setContainsGivenResource);
    }
}

void TestResourceSet::testAddResources()
{
    resourceSet->addResource(audioResource);

    bool setContainsAudioResource = resourceSet->contains(AudioPlaybackType);
    QVERIFY(setContainsAudioResource);
    QList<Resource *> resources;

    resources << audioResource << videoResource;

    resourceSet->addResources(resources);

    QVERIFY(resourceSet->contains(AudioPlaybackType));
    QVERIFY(resourceSet->contains(VideoPlaybackType));
}

void TestResourceSet::testDelResource()
{
    resourceSet->addResource(audioResource);
    resourceSet->addResource(vibraResource);
    resourceSet->delResource(AudioPlaybackType);

    bool setDoesNotContainAudioResource = !resourceSet->contains(AudioPlaybackType);
    QVERIFY(setDoesNotContainAudioResource);
    QVERIFY(resourceSet->contains(VibraType));
}

void TestResourceSet::testContainsSingle()
{
    QList<Resource *> resources;

    resources << audioResource << videoResource << audioRecorderResource;
    resourceSet->addResources(resources);
    QList<Resource *> resourcesInSet = resourceSet->resources();

    bool containsVideoPlayback = resourceSet->contains(VideoPlaybackType);
    QVERIFY(containsVideoPlayback);
}

void TestResourceSet::testDoesNotContainSingle()
{
    QList<Resource *> resources;

    resources << audioResource << videoResource;
    resourceSet->addResources(resources);
    QList<Resource *> resourcesInSet = resourceSet->resources();

    bool doesNotContainVideoRecorder = !resourceSet->contains(VideoRecorderType);
    QVERIFY(doesNotContainVideoRecorder);
}


void TestResourceSet::testContainsSet()
{
    QList<ResourceType> types, subset;
    QList<Resource *> resources;

    types << AudioPlaybackType << VideoPlaybackType
              << AudioRecorderType << VideoRecorderType << LensCoverType;
    subset << AudioPlaybackType << VideoPlaybackType << LensCoverType;

    resources << audioResource << videoResource
              << audioRecorderResource << videoRecorderResource << lensCoverResource;

    resourceSet->addResources(resources);

    bool containsAll = resourceSet->contains(types);
    bool containsSubset = resourceSet->contains(subset);
    QVERIFY(containsAll);
    QVERIFY(containsSubset);
}
QTEST_MAIN(TestResourceSet)

