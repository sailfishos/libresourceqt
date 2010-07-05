#include "test-resource-set.h"

using namespace ResourcePolicy;

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
    case HeadsetButtonsType:
        return new HeadsetButtonsResource;
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

void TestResourceSet::testConnectToSignals()
{
    bool signalConnectionSucceeded=false;
    signalConnectionSucceeded = QObject::connect(resourceSet,
        SIGNAL(resourcesBecameAvailable(const QList<ResourcePolicy::ResourceType> &)),
        this, SLOT(handleResourcesBecameAvailable(const QList<ResourcePolicy::ResourceType> &)));

    QVERIFY(signalConnectionSucceeded);

    signalConnectionSucceeded = QObject::connect(resourceSet,
        SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType> &)),
        this, SLOT(handleResourcesGranted(const QList<ResourcePolicy::ResourceType> &)));

    QVERIFY(signalConnectionSucceeded);

    signalConnectionSucceeded = QObject::connect(resourceSet,
        SIGNAL(resourcesDenied()),
        this, SLOT(handleResourcesDenied()));

    QVERIFY(signalConnectionSucceeded);

    signalConnectionSucceeded = QObject::connect(resourceSet,
        SIGNAL(resourcesReleased()),
        this, SLOT(handleResourcesReleased()));

    QVERIFY(signalConnectionSucceeded);

    signalConnectionSucceeded = QObject::connect(resourceSet,
        SIGNAL(lostResources()),
        this, SLOT(handleLostResources()));

    QVERIFY(signalConnectionSucceeded);

}

void TestResourceSet::handleResourcesBecameAvailable(const QList<ResourcePolicy::ResourceType> &)
{

}
void TestResourceSet::handleResourcesGranted(const QList<ResourcePolicy::ResourceType> &)
{
}

void TestResourceSet::handleResourcesDenied()
{
}

void TestResourceSet::handleResourcesReleased()
{
}

void TestResourceSet::handleLostResources()
{
}

QTEST_MAIN(TestResourceSet)

