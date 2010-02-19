#include <policy/resource-set.h>
#include "resource-engine.h"
using namespace ResourcePolicy;


ResourceSet::ResourceSet(const QString &applicationClass, QObject * parent)
        : QObject(parent), resourceClass(applicationClass), resourceEngine(NULL),
        audioResource(NULL), autoRelease(false), alwaysReply(false),
        initialized(false), pendingAcquire(false), pendingUpdate(false),
        pendingAudioProperties(false)
{
    identifier = (quint32)this;
    memset(resourceSet, 0, sizeof(QPointer<Resource *>)*NumberOfTypes);
}

ResourceSet::~ResourceSet()
{
    for (int i = 0;i < NumberOfTypes;i++) {
        delete resourceSet[i];
    }
    if(resourceEngine != NULL) {
        resourceEngine->disconnect(this);
        //resourceEngine->disconnectFromManager();
        delete resourceEngine;
    }
}

bool ResourceSet::initialize()
{
    resourceEngine = new ResourceEngine(this);
    if (resourceEngine == NULL) {
        return false;
    }
    QObject::connect(resourceEngine, SIGNAL(connectedToManager()),
                     this, SLOT(connectedHandler()));
    QObject::connect(resourceEngine, SIGNAL(resourcesGranted(quint32)),
                     this, SLOT(handleGranted(quint32)));
    QObject::connect(resourceEngine, SIGNAL(resourcesDenied()),
                     this, SLOT(handleDeny()));
    QObject::connect(resourceEngine, SIGNAL(resourcesReleased()),
                     this, SLOT(handleReleased()));
    QObject::connect(resourceEngine, SIGNAL(resourcesLost(quint32)),
                     this, SLOT(handleResourcesLost(quint32)));
    QObject::connect(resourceEngine, SIGNAL(resourcesBecameAvailable(quint32)),
                     this, SLOT(handleResourcesBecameAvailable(quint32)));
    if (!resourceEngine->initialize()) {
        return false;
    }
    if (!resourceEngine->connectToManager()) {
        return false;
    }
    qDebug("ResourceSet is initialized engine=%p", resourceEngine);
    initialized = true;
    return true;
}

void ResourceSet::addResourceObject(Resource *resource)
{
    if(resource == NULL)
        return;
    if (resource->type() == AudioPlaybackType) {
        qDebug("audioResource... %p", resource);
        audioResource = static_cast<AudioResource *>(resource);
        QObject::connect(audioResource,
                          SIGNAL(audioPropertiesChanged(const QString &, quint32,
                                                         const QString &, const QString &)),
                          this,
                          SLOT(handleAudioPropertiesChanged(const QString &, quint32,
                                                             const QString &, const QString &)));
        if(audioResource->streamTagIsSet() || audioResource->audioGroupIsSet() ||
           (audioResource->processID() > 0))
        {
            pendingAudioProperties = true;
        }
    }
    delete resourceSet[resource->type()];
    resourceSet[resource->type()] = resource;
}

bool ResourceSet::addResource(ResourceType type)
{
    Resource *resource = NULL;
    switch (type) {
        case AudioPlaybackType:
            resource = new AudioResource;
            break;
        case AudioRecorderType:
            resource = new AudioRecorderResource;
            break;
        case VideoPlaybackType:
            resource = new VideoResource;
            break;
        case VideoRecorderType:
            resource = new VideoRecorderResource;
            break;
        case VibraType:
            resource = new VibraResource;
            break;
        case LedsType:
            resource = new LedsResource;
            break;
        case BacklightType:
            resource = new BacklightResource;
            break;
        case SystemButtonType:
            resource = new SystemButtonResource;
            break;
        case LockButtonType:
            resource = new LockButtonResource;
            break;
        case ScaleButtonType:
            resource = new ScaleButtonResource;
            break;
        case SnapButtonType:
            resource = new SnapButtonResource;
            break;
        case LensCoverType:
            resource = new LensCoverResource;
            break;
        default:
            return false;
    }
    if (resource == NULL) {
        return false;
    }
    addResourceObject(resource);
    return true;
}

void ResourceSet::deleteResource(ResourceType type)
{
    if(type == AudioPlaybackType) {
        audioResource->disconnect();
        audioResource = NULL;
    }
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
    if (!initialized) {
        pendingAcquire = true;
        return initialize();
    }

    if (!resourceEngine->isConnectedToManager()) {
        pendingAcquire = true;
        resourceEngine->connectToManager();
        return true;
    }
    else {
        return resourceEngine->acquireResources();
    }
}

bool ResourceSet::release()
{
    if (!initialized || !resourceEngine->isConnectedToManager()) {
        return true;
    }

    return resourceEngine->releaseResources();
}

bool ResourceSet::update()
{
    if (!initialized) {
        return true;
    }

    if (!resourceEngine->isConnectedToManager()) {
        pendingUpdate = true;
        return true;
    }
    return resourceEngine->updateResources();
}

QString ResourceSet::applicationClass()
{
    return this->resourceClass;
}

bool ResourceSet::setAutoRelease()
{
    if(initialized)
        return false;
    autoRelease = true;
    return true;
}

bool ResourceSet::willAutoRelease()
{
    return autoRelease;
}

bool ResourceSet::setAlwaysReply()
{
    if(initialized)
        return false;
    alwaysReply = true;
    return true;
}

bool ResourceSet::alwaysGetReply()
{
    return alwaysReply;
}

void ResourceSet::connectedHandler()
{
    if (pendingAudioProperties) {
        qDebug() << "registering audio group: " << audioResource->audioGroup();
        qDebug() << "registering PID: " << audioResource->processID();
        qDebug() << "registering audio stream: " << audioResource->streamTagName()
        << ":" << audioResource->streamTagValue();
        resourceEngine->registerAudioProperties(audioResource->audioGroup(),
                                                audioResource->processID(),
                                                audioResource->streamTagName(),
                                                audioResource->streamTagValue());
        pendingAudioProperties = false;
    }
    if (pendingUpdate) {
        resourceEngine->updateResources();
        pendingUpdate = false;
    }
    if (pendingAcquire) {
        resourceEngine->acquireResources();
        pendingAcquire = false;
    }
}

void ResourceSet::handleGranted(quint32 bitmaskOfGrantedResources)
{
    qDebug("in %s",__FUNCTION__);
    QList<ResourceType> optionalResources;
    qDebug("Acquired resources: 0x%04x", bitmaskOfGrantedResources);
    for(int i=0;i < NumberOfTypes; i++) {
        if(resourceSet[i] == NULL)
            continue;
        ResourceType type = (ResourceType)i;
        quint32 bitmask = resourceTypeToLibresourceType(type);
        qDebug("Checking if resource 0x%04x is in the set", bitmask);
        if ((bitmask & bitmaskOfGrantedResources) == bitmask) {
            if (resourceSet[i]->isOptional()) {
                optionalResources << type;
            }
            resourceSet[i]->setGranted();
            qDebug("Resource 0x%04x is now granted", i);
        }
        else {
            resourceSet[i]->unsetGranted();
        }
    }
    emit resourcesGranted(optionalResources);
}

void ResourceSet::handleReleased()
{
    for(int i=0;i < NumberOfTypes; i++) {
        if(resourceSet[i] != NULL) {
            resourceSet[i]->unsetGranted();
        }
    }
    emit resourcesReleased();
}

void ResourceSet::handleDeny()
{
    for(int i=0;i < NumberOfTypes; i++) {
        if(resourceSet[i] != NULL) {
            resourceSet[i]->unsetGranted();
        }
    }
    emit resourcesDenied();
}

void ResourceSet::handleResourcesLost(quint32 lostResourcesBitmask)
{
    for(int i=0;i < NumberOfTypes; i++) {
        quint32 bitmask = resourceTypeToLibresourceType((ResourceType)i);
        if ((bitmask & lostResourcesBitmask) == bitmask) {
            resourceSet[i]->unsetGranted();
            qDebug("Resource %04x is now lost", bitmask);
        }
    }
    emit lostResources();
}

void ResourceSet::handleResourcesBecameAvailable(quint32 availableResources)
{
    QList<ResourceType> listOfResources;
    for (int i=0;i < NumberOfTypes; i++) {
        ResourceType type = (ResourceType)i;
        quint32 bitmask = resourceTypeToLibresourceType(type);
        if ((bitmask & availableResources) == bitmask) {
            listOfResources.append(type);
        }
    }
    emit resourcesBecameAvailable(listOfResources);
}

void ResourceSet::handleAudioPropertiesChanged(const QString &group, quint32 pid,
                                                const QString &name, const QString &value)
{
    qDebug() << "Audio group is: " << group;
    qDebug("Audio renderer PID = %u", pid);
    if (initialized && resourceEngine->isConnectedToManager()) {
        qDebug("registering new audio settings");
        resourceEngine->registerAudioProperties(group, pid, name, value);
    }
    else {
        qDebug("registering new audio group LATER");
        pendingAudioProperties = true;
    }
}
