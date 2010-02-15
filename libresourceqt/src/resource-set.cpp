#include <policy/resource-set.h>
#include "resource-engine.h"
using namespace ResourcePolicy;


ResourceSet::ResourceSet(const QString &applicationClass, QObject * parent)
        : QObject(parent), resourceClass(applicationClass), resourceEngine(NULL),
        autoRelease(false), alwaysReply(false), initialized(false),
        pendingAcquire(false)
{
    identifier = (quint32)this;
    memset(resourceSet, 0, sizeof(QPointer<Resource *>)*NumberOfTypes);
}

ResourceSet::~ResourceSet()
{
    for (int i = 0;i < NumberOfTypes;i++) {
        delete resourceSet[i];
    }
}

bool ResourceSet::initialize()
{
    resourceEngine = new ResourceEngine(this);
    if (resourceEngine == NULL) {
        return false;
    }
    if (!resourceEngine->initialize()) {
        return false;
    }
    if (!resourceEngine->connect()) {
        return false;
    }
    QObject::connect(resourceEngine, SIGNAL(connectedToManager()), 
                     this, SLOT(connectedHandler()));
    QObject::connect(resourceEngine, SIGNAL(resourcesAcquired(quint32)),
                     this, SLOT(handleAcquire(quint32)));
    QObject::connect(resourceEngine, SIGNAL(resourcesDenied()),
                     this, SLOT(handleDeny()));

    return true;
}

void ResourceSet::addResource(const Resource *resource)
{
    if ((resource->type() == AudioPlaybackType) || (resource->type() == AudioRecorderType)) {
        qDebug("audioResource...");
    }
    delete resourceSet[resource->type()];
    resourceSet[resource->type()] = resource->clone();
}

void ResourceSet::addResources(const QList<Resource *>resources)
{
    for (int i = 0; i < resources.size(); i++) {
        addResource(resources.at(i));
    }
}

void ResourceSet::deleteResource(ResourceType type)
{
    delete resourceSet[type];
    resourceSet[type] = NULL;
}

bool ResourceSet::contains(ResourceType type) const
{
    if ((type < NumberOfTypes) && (resourceSet[type] != NULL))
        return true;
    else
        return false;
}

bool ResourceSet::contains(const QList<ResourceType> &types) const
{
    bool containsAll = true;
    int i = 0;
    do {
        containsAll = contains(types.at(i));
        i++;
    }
    while ((i < types.size()) && containsAll);
    return containsAll;
}

quint32 ResourceSet::id() const
{
    return identifier;
}

QList<Resource *> ResourceSet::resources() const
{
    QList<Resource *> listOfResources;
    for (int i = 0; i < NumberOfTypes; i++) {
        if (resourceSet[i] != NULL) {
            listOfResources.append(resourceSet[i]);
        }
    }
    return listOfResources;
}

Resource * ResourceSet::resource(ResourceType type) const
{
    return resourceSet[type];
}

bool ResourceSet::acquire()
{
    if(!initialized) {
        pendingAcquire = true;
        return initialize();
    }
    else if (!resourceEngine->isConnected()) {
        pendingAcquire = true;
        resourceEngine->connect();
    }
    else {
        return resourceEngine->acquireResources();
    }
}

bool ResourceSet::release()
{
    return false;
}

bool ResourceSet::update()
{
    return false;
}

QString ResourceSet::applicationClass()
{
    return this->resourceClass;
}

void ResourceSet::setAutoRelease()
{
    autoRelease = true;
}

void ResourceSet::setAlwaysReply()
{
    alwaysReply = true;
}

void ResourceSet::connectedHandler()
{
    if (pendingAcquire) {
        resourceEngine->acquireResources();
    }
}

void ResourceSet::handleAcquire(quint32 bitmaskOfGrantedResources)
{
    QList<ResourceType> optionalResources;
    qDebug("Acquired resources: 0x%04x", bitmaskOfGrantedResources);
    for(int i=0;i < NumberOfTypes; i++) {
        ResourceType type = (ResourceType)i;
        quint32 bitmask = resourceTypeToLibresourceType(type);
        qDebug("Checking if resource %x(%x) is in the set", i, bitmask);
        if ((bitmask & bitmaskOfGrantedResources) == bitmask) {
            if (resourceSet[i]->isOptional()) {
                optionalResources << type;
            }
            resourceSet[i]->setGranted();
        }
    }
    emit resourcesGranted(optionalResources);
}

void ResourceSet::handleDeny()
{
    for(int i=0;i < NumberOfTypes; i++) {
        if(resourceSet[i] != NULL) {
            resourceSet[i]->unsetGranted();
        }
    }
}

