#include "test-resource-set.h"

Resource * TestResourceSet::resourceFromType(ResourceType type)
{
    switch (type) {
    case AudioPlaybackType:
        return new AudioResource;
    case AudioRecorderType:
        return new AudioRecorderResource;
    case VideoPlaybackType:
        return new VideoResource;
    case VideoRecorderType:
        return new VideoRecorderResource;
    case VibraType:
        return new VibraResource;
    case LedsType:
        return new LedsResource;
    case BacklightType:
        return new BacklightResource;
    case SystemButtonType:
        return new SystemButtonResource;
    case LockButtonType:
        return new LockButtonResource;
    case ScaleButtonType:
        return new ScaleButtonResource;
    case SnapButtonType:
        return new SnapButtonResource;
    case LensCoverType:
        return new LensCoverResource;
    default:
        return NULL;
    }
}

using namespace ResourcePolicy;

TestResourceSet::TestResourceSet()
{
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
    for (int i = 0;i < NumberOfTypes;i++) {
        ResourceType type = (ResourceType)i;
        resourceSet->addResource(type);
        bool setContainsGivenResource = resourceSet->contains(type);
        QVERIFY(setContainsGivenResource);
    }
}

void TestResourceSet::testAddResourceObject()
{
    for (int i = 0;i < NumberOfTypes;i++) {
        ResourceType type = (ResourceType)i;
        Resource *resource = resourceFromType(type);
        resourceSet->addResourceObject(resource);
        bool setContainsGivenResource = resourceSet->contains(type);
        QVERIFY(setContainsGivenResource);
    }
}

void TestResourceSet::testDelResource()
{
    for (int i = 0;i < NumberOfTypes;i++) {
        ResourceType type = (ResourceType)i;
        resourceSet->addResource(type);
        bool setContainsGivenResource = resourceSet->contains(type);
        QVERIFY(setContainsGivenResource);
    }

    for (int i = 0;i < NumberOfTypes;i++) {
        ResourceType type = (ResourceType)i;
        resourceSet->deleteResource(type);
        bool setNoLongerContainType = !resourceSet->contains(type);
        QVERIFY(setNoLongerContainType);
        for (int j = 0; j < NumberOfTypes; j++) {
            if(j == i) continue;
            ResourceType otherType = (ResourceType)j;
            bool setStillContainsOtherTypes = resourceSet->contains(otherType);
            QVERIFY(setStillContainsOtherTypes);
        }
        resourceSet->addResource(type);
    }
}

void TestResourceSet::testContainsSet()
{
    QList<ResourceType> types, subset;

    for (int i = 0;i < NumberOfTypes;i++) {
        ResourceType type = (ResourceType)i;
        resourceSet->addResource(type);
        types.append(type);
    }

    subset << AudioPlaybackType << VideoPlaybackType
    << AudioRecorderType << VideoRecorderType << LensCoverType;

    bool containsAll = resourceSet->contains(types);
    bool containsSubset = resourceSet->contains(subset);
    QVERIFY(containsAll);
    QVERIFY(containsSubset);
}
QTEST_MAIN(TestResourceSet)

