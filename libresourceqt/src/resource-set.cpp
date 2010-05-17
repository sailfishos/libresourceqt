#include <policy/resource-set.h>
#include "resource-engine.h"
using namespace ResourcePolicy;

static quint32 resourceSetId=1;

ResourceSet::ResourceSet(const QString &applicationClass, QObject * parent)
        : QObject(parent), resourceClass(applicationClass), resourceEngine(NULL),
        audioResource(NULL), autoRelease(false), alwaysReply(false),
        initialized(false), pendingAcquire(false), pendingUpdate(false),
        pendingAudioProperties(false)
{
    identifier = resourceSetId++;
    memset(resourceSet, 0, sizeof(QPointer<Resource *>)*NumberOfTypes);
}

ResourceSet::~ResourceSet()
{
    qDebug("ResourceSet::%s(%d)", __FUNCTION__, identifier);
    for (int i = 0;i < NumberOfTypes;i++) {
        delete resourceSet[i];
    }
    if(resourceEngine != NULL) {
        qDebug("ResourceSet::%s(%d) - resourceEngine->disconnectFromManager()", __FUNCTION__, identifier);
        resourceEngine->disconnect(this);
        resourceEngine->disconnectFromManager();
    }
    qDebug("ResourceSet::%s(%d) - deleted!", __FUNCTION__, identifier);
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
    qDebug("initializing resource engine...");
    if (!resourceEngine->initialize()) {
        return false;
    }
    qDebug("resourceEngine->initialize() returned true");
    if (!resourceEngine->connectToManager()) {
        return false;
    }
    qDebug("ResourceSet is initialized engine=%p", resourceEngine);
    initialized = true;
    qDebug("**************** ResourceSet::%s().... %d", __FUNCTION__, __LINE__);
    return true;
}

void ResourceSet::addResourceObject(Resource *resource)
{
    qDebug("**************** ResourceSet::%s().... %d", __FUNCTION__, __LINE__);
    if(resource == NULL)
        return;
    qDebug("**************** ResourceSet::%s().... %d", __FUNCTION__, __LINE__);
    delete resourceSet[resource->type()];
    resourceSet[resource->type()] = resource;
    if ((resource->type() == AudioPlaybackType)) {
        qDebug("**************** ResourceSet::%s().... %d", __FUNCTION__, __LINE__);
        audioResource = static_cast<AudioResource *>(resource);
        QObject::connect(audioResource,
                          SIGNAL(audioPropertiesChanged(const QString &, quint32,
                                                         const QString &, const QString &)),
                          this,
                          SLOT(handleAudioPropertiesChanged(const QString &, quint32,
                                                             const QString &, const QString &)));
        if(audioResource->streamTagIsSet() && (audioResource->processID() > 0))
        {
            qDebug("registering audio properties");
            registerAudioProperties();
        }
        else if (audioResource->audioGroupIsSet()) {
            qDebug("ResourceSet::%s().... %d registering audio proprerties later", __FUNCTION__, __LINE__);
            pendingAudioProperties = true;            
        }
    }
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
        qDebug("**************** ResourceSet::%s().... %d", __FUNCTION__, __LINE__);
        pendingAcquire = true;
        return initialize();
    }

    if (!resourceEngine->isConnectedToManager()) {
        qDebug("**************** ResourceSet::%s().... %d", __FUNCTION__, __LINE__);
        pendingAcquire = true;
        resourceEngine->connectToManager();
        return true;
    }
    else {
        qDebug("**************** ResourceSet::%s().... %d", __FUNCTION__, __LINE__);
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
    qDebug("**************** ResourceSet::%s().... %d", __FUNCTION__, __LINE__);
    qDebug("Connected to manager!");
    if (pendingAudioProperties) {
        registerAudioProperties();
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

void ResourceSet::registerAudioProperties()
{
    if (!initialized) {
        qDebug("%s(): initializing...", __FUNCTION__);
        pendingAudioProperties = true;
        initialize();
    }
    else if (!resourceEngine->isConnectedToManager()) {
        qDebug("%s(): Connecting to Manager...", __FUNCTION__);
        pendingAudioProperties = true;
        resourceEngine->connectToManager();
    }
    else {
        qDebug("Registering new audio settings:");
        qDebug() << "\taudio group: " << audioResource->audioGroup();
        qDebug() << "\tPID: " << audioResource->processID();
        qDebug() << "\taudio stream: " << audioResource->streamTagName()
                 << ":" << audioResource->streamTagValue();
        if((audioResource->processID() > 0) && audioResource->streamTagName() != "media.name") {
            qWarning() << "streamTagName should be 'media.name' it is '" << audioResource->streamTagName() << "'";
        }
        resourceEngine->registerAudioProperties(audioResource->audioGroup(),
                                                audioResource->processID(),
                                                audioResource->streamTagName(),
                                                audioResource->streamTagValue());
        pendingAudioProperties = false;
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

void ResourceSet::handleAudioPropertiesChanged(const QString &, quint32,
                                                const QString &, const QString &)
{
    registerAudioProperties();
}

