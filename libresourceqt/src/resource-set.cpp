/*************************************************************************
This file is part of libresourceqt

Copyright (C) 2010 Nokia Corporation.

This library is free software; you can redistribute
it and/or modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation
version 2.1 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
USA.
*************************************************************************/
#include <policy/resource-set.h>
#include "resource-engine.h"
using namespace ResourcePolicy;

static quint32 resourceSetId=1;

class ResourceSetPrivate
{
public:
    ResourceSetPrivate();
};


ResourceSet::ResourceSet(const QString &applicationClass, QObject * parent,
                         bool initialAlwaysReply, bool initialAutoRelease)
        : QObject(parent), resourceClass(applicationClass), resourceEngine(NULL),
        audioResource(NULL), autoRelease(initialAutoRelease),
        alwaysReply(initialAlwaysReply), initialized(false), pendingAcquire(false),
        pendingUpdate(false), pendingAudioProperties(false), pendingVideoProperties(false),
        inAcquireMode(false), reqMutex(QMutex::Recursive), ignoreQ(false)
{
    identifier = resourceSetId++;
    memset(resourceSet, 0, sizeof(QPointer<Resource *>)*NumberOfTypes);
}

ResourceSet::ResourceSet(const QString &applicationClass, QObject * parent)
        : QObject(parent), resourceClass(applicationClass), resourceEngine(NULL),
        audioResource(NULL), autoRelease(false),
        alwaysReply(false), initialized(false), pendingAcquire(false),
        pendingUpdate(false), pendingAudioProperties(false), pendingVideoProperties(false),
        inAcquireMode(false), reqMutex(QMutex::Recursive), ignoreQ(false)
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
    QObject::connect(resourceEngine, SIGNAL(errorCallback(quint32, const char*)),
                     this, SIGNAL(errorCallback(quint32, const char*)));
    QObject::connect(resourceEngine, SIGNAL(resourcesReleasedByManager()),
                     this, SLOT(handleReleasedByManager()));
    QObject::connect(resourceEngine, SIGNAL(updateOK(bool)),
                     this, SLOT(handleUpdateOK(bool)));

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
    qDebug("**************** ResourceSet::%s(%d).... %d", __FUNCTION__,this->id(), __LINE__);
    if(resource == NULL)
        return;
    qDebug("**************** ResourceSet::%s(%d).... %d", __FUNCTION__,this->id(), __LINE__);
    delete resourceSet[resource->type()];
    resourceSet[resource->type()] = resource;

    if ( resource->type() == AudioPlaybackType ) {

        qDebug("**************** ResourceSet::%s(%d).... %d", __FUNCTION__,this->id(), __LINE__);
        audioResource = static_cast<AudioResource *>(resource);
        QObject::connect(audioResource,
                          SIGNAL(audioPropertiesChanged(const QString &, quint32,
                                                         const QString &, const QString &)),
                          this,
                          SLOT(handleAudioPropertiesChanged(const QString &, quint32,
                                                             const QString &, const QString &)));
        if (!audioResource->audioGroupIsSet())
            audioResource->setAudioGroup(resourceClass);

        if (audioResource->streamTagIsSet() && (audioResource->processID() > 0))
        {
            qDebug("registering audio properties");
            registerAudioProperties();
        }
        else if (audioResource->audioGroupIsSet()) {
            qDebug("ResourceSet::%s().... %d registering audio proprerties later", __FUNCTION__, __LINE__);
            pendingAudioProperties = true;
        }

    }
    else if ( resource->type() == VideoPlaybackType ) {

        qDebug("**************** ResourceSet::%s(%d).... %d", __FUNCTION__,this->id(), __LINE__);
        videoResource = static_cast<VideoResource *>(resource);

        QObject::connect(videoResource,
                          SIGNAL(videoPropertiesChanged(quint32)),
                          this,
                          SLOT(handleVideoPropertiesChanged(quint32)));
        if (videoResource->processID() > 0)
        {
            qDebug("registering video properties");
            registerVideoProperties();
        }
    }


    if (resourceEngine &&
       (resourceEngine->isConnectedToManager() || resourceEngine->isConnectingToManager()) )
    {
        pendingUpdate = true;
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
        case HeadsetButtonsType:
            resource = new HeadsetButtonsResource;
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
        pendingAudioProperties = false;
    }
    delete resourceSet[type];
    resourceSet[type] = NULL;

    if (resourceEngine &&
       (resourceEngine->isConnectedToManager() || resourceEngine->isConnectingToManager()) )
    {
        pendingUpdate = true;
    }

}

bool ResourceSet::contains(ResourceType type) const
{
    if ((type < NumberOfTypes) && (resourceSet[type] != NULL))
        return true;
    else
        return false;
}

bool ResourceSet::isConnectedToManager() const
{
    if (resourceEngine == NULL) {
        // not initialized
        return false;
    }
    return resourceEngine->isConnectedToManager();
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

bool ResourceSet::initAndConnect()
{
    if ( !initialized ) {
        qDebug("ResourceSet::%s().... initializing...", __FUNCTION__);
        return initialize();
    }
    if ( !resourceEngine->isConnectedToManager() ) {
        qDebug("ResourceSet::%s().... connecting...", __FUNCTION__);
        return resourceEngine->connectToManager();
    }
    else
        qDebug("ResourceSet::%s(): already connected", __FUNCTION__);

    return true;
}


bool ResourceSet::proceedIfImFirst( requestType theRequest )
{    
    if  (!ignoreQ)
        requestQ.push_back( theRequest );
    else
    {
        qDebug("ResourceSet::%s()...executing first request of %d.", __FUNCTION__, requestQ.size() );
        return true;
    }

    //Execute if this is the first request or the next is run from slot.
    if ( requestQ.size() == 1  )
    {
        if (!ignoreQ) { qDebug("ResourceSet::%s()...allowing only request directly.", __FUNCTION__); }
        return true;
    }

    if ( requestQ.size() > 1 )
    {
        qDebug("ResourceSet::%s()...queuing request %d.", __FUNCTION__, requestQ.size());

        switch (theRequest)
        {
        case Acquire:  qDebug("ResourceSet::%s()...queuing request:Acquire.", __FUNCTION__); break;
        case Update:   qDebug("ResourceSet::%s()...queuing request:Update.", __FUNCTION__);  break;
        case Release:  qDebug("ResourceSet::%s()...queuing request:Release.", __FUNCTION__); break;
        }
        return false;
    }

    Q_ASSERT_X(0, "proceedIfImFirst", "request queue can not be empty.");

    return false;
}


void ResourceSet::executeNextRequest()
{
    qDebug("ResourceSet::%s().", __FUNCTION__);

    if ( requestQ.isEmpty() )
    {
        qDebug("ResourceSet::%s()...the completed request is not present.",
               __FUNCTION__);
        return;
    }

    requestQ.removeFirst(); //Remove completed request.

    if ( requestQ.isEmpty() )
    {
        qDebug("ResourceSet::%s()...last request acknowledged and removed.", __FUNCTION__);
        return;
    }

    requestType nxtReq = requestQ.at(0);

    //Ensure that proceedIfimFirst() lets through.
    ignoreQ = true;
    //Having recursive mutexes, because it is taken again in proceedIfImFirst.
    qDebug("ResourceSet::%s()...executing first request of %d.", __FUNCTION__, requestQ.size() );

    switch (nxtReq)
    {
    case Acquire: qDebug("ResourceSet::%s()...Acquire.", __FUNCTION__); this->acquire();  break;
    case Update:  qDebug("ResourceSet::%s()...Update.",  __FUNCTION__); this->update();   break;
    case Release: qDebug("ResourceSet::%s()...Release.", __FUNCTION__); this->release();  break;
    }

    ignoreQ = false;

    //Q_ASSERT_X(0, "executeNextRequest", "should not happen since requestQ.isEmpty() was false.");

}



bool ResourceSet::acquire()
{

    if ( !initialized || !resourceEngine->isConnectedToManager() )
    {
        pendingAcquire = true;
        return initAndConnect();
    }
    else
    {
      /*  if (pendingUpdate)
        { //Connected and there are res.added.

            if ( !proceedIfImFirst( Update, Acquire ) ) return true;

            qDebug("ResourceSet::%s().... forcing update.", __FUNCTION__);

            if (!resourceEngine->updateResources()) return false;

            if ( inAcquireMode ) return true;
        }*/

        if ( !proceedIfImFirst( Acquire ) ) return true;

        qDebug("ResourceSet::%s().... acquiring", __FUNCTION__);
        return resourceEngine->acquireResources();

    }
}

bool ResourceSet::release()
{
    if (!initialized || !resourceEngine->isConnectedToManager()) {
        return true;
    }

    if ( !proceedIfImFirst( Release ) ) return true;

    //inAcquireMode = false;
    qDebug("ResourceSet::%s().... releasing...", __FUNCTION__);
    return resourceEngine->releaseResources();
}

bool ResourceSet::update()
{
    if (!initialized) {
        return true;
    }

    if (!resourceEngine->isConnectedToManager()) {
        pendingUpdate = true;
        resourceEngine->connectToManager();
        return true;
    }

    if ( !proceedIfImFirst( Update ) ) return true;

    qDebug("ResourceSet::%s().... updating...", __FUNCTION__);
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
    if (resourceEngine->isConnectedToManager()) {
        qDebug("ResourceSet::%s() Connected to manager!", __FUNCTION__);
        emit managerIsUp();

        if (pendingAudioProperties) {
            registerAudioProperties();
        }
        if (pendingVideoProperties) {
            registerVideoProperties();
        }
        if (pendingUpdate) {
            resourceEngine->updateResources();
            pendingUpdate = false;
        }
        if (pendingAcquire) {
            acquire();
            pendingAcquire = false;
        }
    }
    else { // assuming reconnecting
        qDebug("ResourceSet::%s() Reconnecting to manager...", __FUNCTION__);

        // first check if we have any acquired resources
        for (int i = 0; i < NumberOfTypes; i++) {
            if (resourceSet[i] != NULL) {
                if (resourceSet[i]->isGranted()) {

                    if (i == AudioPlaybackType) {
                        pendingAudioProperties = true;
                        qDebug("ResourceSet::%s() We have audio", __FUNCTION__);
                    }

                    if (i == VideoPlaybackType) {
                        pendingVideoProperties = true;
                        qDebug("ResourceSet::%s() We have video", __FUNCTION__);
                    }

                    qDebug("ResourceSet::%s() We have acquired resources. Re-acquire", __FUNCTION__);
                    pendingAcquire = true;
                    resourceSet[i]->unsetGranted();
                }
            }
        }
        // now reconnect
        resourceEngine->connectToManager();
    }
}

void ResourceSet::registerAudioProperties()
{
    if (!initialized) {
        qDebug("%s(): initializing...", __FUNCTION__);
        pendingAudioProperties = true;
        initialize();
        return;
    }
    else if (resourceEngine->isConnectedToManager()) {
        qDebug("Registering new audio settings:");
        qDebug() << "\taudio group: " << audioResource->audioGroup();
        qDebug() << "\tPID: " << audioResource->processID();
        qDebug() << "\taudio stream: " << audioResource->streamTagName()
                 << ":" << audioResource->streamTagValue();
        if((audioResource->processID() > 0) && audioResource->streamTagName() != "media.name") {
            qWarning() << "streamTagName should be 'media.name' it is '" << audioResource->streamTagName() << "'";
        }
        bool r = resourceEngine->registerAudioProperties(audioResource->audioGroup(),
                                                         audioResource->processID(),
                                                         audioResource->streamTagName(),
                                                         audioResource->streamTagValue());
        qDebug("resourceEngine->registerAudioProperties returned %s", r?"true":"false");

        pendingAudioProperties = false;
    }
    else { //if (!resourceEngine->isConnectedToManager() && !resourceEngine->isConnectingToManager()) {
        qDebug("%s(): Connecting to Manager...", __FUNCTION__);

        pendingAudioProperties = true;
        resourceEngine->connectToManager();
        return;
    }
}

void ResourceSet::registerVideoProperties()
{
    if (!initialized) {
        qDebug("%s(): initializing...", __FUNCTION__);
        pendingVideoProperties = true;
        initialize();
        return;
    }
    else if (resourceEngine->isConnectedToManager()) {

        qDebug("Registering new video settings:");
        qDebug() << "\tPID: " << videoResource->processID();

        if( videoResource->processID() < 2 ) {
            qWarning() << "processID should be > 1 '" << "'";
        }

        bool r = resourceEngine->registerVideoProperties( videoResource->processID() );

        qDebug("resourceEngine->registerVideoProperties returned %s", r?"true":"false");

        pendingVideoProperties = false;
    }
    else { //if (!resourceEngine->isConnectedToManager() && !resourceEngine->isConnectingToManager()) {
        qDebug("%s(): Connecting to Manager...", __FUNCTION__);

        pendingVideoProperties = true;
        resourceEngine->connectToManager();
        return;
    }
}

void ResourceSet::handleGranted(quint32 bitmaskOfGrantedResources)
{
    qDebug(" ResourceSet::%s",__FUNCTION__);
    QList<ResourceType> optionalResources;
    qDebug("Acquired resources: 0x%04x", bitmaskOfGrantedResources);

    bool setChanged   = false;

    for(int i=0;i < NumberOfTypes; i++) {

        if(resourceSet[i] == NULL)
            continue;

        ResourceType type = (ResourceType)i;
        quint32 bitmask   = resourceTypeToLibresourceType(type);
        qDebug("Checking if resource 0x%04x is in the set", bitmask);

        if ((bitmask & bitmaskOfGrantedResources) == bitmask) {
            if (resourceSet[i]->isOptional()) {
                optionalResources << type;
            }
            if ( !resourceSet[i]->isGranted() )
                setChanged = true;

            resourceSet[i]->setGranted();            
            qDebug("Resource 0x%04x is now granted", i);
        }
        else
        {
            if ( resourceSet[i]->isGranted() )
                setChanged = true;

            resourceSet[i]->unsetGranted();
            setChanged = true;
        }
    }

    //When we come to this slot bitmaskOfGrantedResources contains resources.
    if ( alwaysReply || ( !alwaysReply && setChanged ) ) {
        qDebug(" ResourceSet::%s - emitting resourcesGranted(optionalResources) ",__FUNCTION__);
        emit resourcesGranted(optionalResources);
    }

    inAcquireMode = true;
    executeNextRequest();
}

void ResourceSet::handleReleased()
{
    for(int i=0;i < NumberOfTypes; i++) {
        if(resourceSet[i] != NULL) {
            resourceSet[i]->unsetGranted();
        }
    }

    if ( alwaysReply || ( !alwaysReply && inAcquireMode)  ) emit resourcesReleased();

    qDebug("ResourceSet(%d) - resourcesReleased!", identifier);
    inAcquireMode = false;

    executeNextRequest();
    //emit resourcesReleased();
}

void ResourceSet::handleDeny()
{
    for(int i=0;i < NumberOfTypes; i++) {
        if(resourceSet[i] != NULL) {
            resourceSet[i]->unsetGranted();
        }
    }
    executeNextRequest();
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

    //All requests are invalid when we are pre-empted.
   requestQ.clear();
    if (inAcquireMode) emit lostResources();

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

void ResourceSet::handleVideoPropertiesChanged( quint32)
{
    registerVideoProperties();
}

void ResourceSet::handleReleasedByManager()
{
    //All requests are invalid when we are pre-empted.
   requestQ.clear();

   resourceEngine->releaseResources();
   inAcquireMode = false;
   emit resourcesReleasedByManager();
}

void ResourceSet::handleUpdateOK(bool resend)
{    
    pendingUpdate = false;
    qDebug("ResourceSet::%s().... %d", __FUNCTION__, __LINE__);

    if ( resend ) {

        /*QList<ResourceType> optionalResources;

        for (int i=0; i < NumberOfTypes; i++) {
            if(resourceSet[i] == NULL)
                continue;

            ResourceType type = (ResourceType)i;
            if ( resourceSet[i]->isOptional() &&  resourceSet[i]->isGranted() )
                optionalResources << type;
        }*/

        //Only way to reply if alwaysReply is off and the set doesn't change.
        emit updateOK();
    }

    qDebug("ResourceSet::%s()...about to exe next request....", __FUNCTION__);
    executeNextRequest();

}
