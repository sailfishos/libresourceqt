#ifndef TEST_RESOURCE_SET_H
#define TEST_RESOURCE_SET_H

#include <QtTest/QTest>
#include <policy/resource-set.h>

class TestResourceSet: public QObject
{
    Q_OBJECT
private:
    ResourcePolicy::ResourceSet *resourceSet;

    ResourcePolicy::AudioResource *audioResource;
    ResourcePolicy::AudioRecorderResource *audioRecorderResource;
    ResourcePolicy::Resource *videoResource;
    ResourcePolicy::Resource *videoRecorderResource;
    ResourcePolicy::Resource *vibraResource;
    ResourcePolicy::Resource *ledsResource;
    ResourcePolicy::Resource *backlightResource;
    ResourcePolicy::Resource *systemButtonResource;
    ResourcePolicy::Resource *lockButtonResource;
    ResourcePolicy::Resource *scaleButtonResource;
    ResourcePolicy::Resource *snapButtonResource;
    ResourcePolicy::Resource *lensCoverResource;

    ResourcePolicy::Resource * resourceFromType(ResourcePolicy::ResourceType type);
public:
    TestResourceSet();
    ~TestResourceSet();

public slots:

    void handleResourcesBecameAvailable(const QList<ResourcePolicy::ResourceType> &availableResources);
    void handleResourcesGranted(const QList<ResourcePolicy::ResourceType> &grantedOptionalResources);
    void handleResourcesDenied();
    void handleResourcesReleased();
    void handleLostResources();

private slots:

    void init();
    void cleanup();

    void testIdentifier();

    void testAddResource();
    void testAddResourceObject();
    void testDelResource();

    void testContainsSet();

    void testConnectToSignals();
};

#endif
