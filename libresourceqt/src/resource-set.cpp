/*************************************************************************
This file is part of libresourceqt

Copyright (C) 2011 Nokia Corporation.

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
    memset(resourceSet, 0, sizeof(Resource *)*NumberOfTypes);
}

ResourceSet::ResourceSet(const QString &applicationClass, QObject * parent)
    : QObject(parent), resourceClass(applicationClass), resourceEngine(NULL),
      audioResource(NULL), autoRelease(false),
      alwaysReply(false), initialized(false), pendingAcquire(false),
      pendingUpdate(false), pendingAudioProperties(false), pendingVideoProperties(false),
      inAcquireMode(false), reqMutex(QMutex::Recursive), ignoreQ(false)
{
    identifier = resourceSetId++;
    memset(resourceSet, 0, sizeof(Resource *)*NumberOfTypes);
}

ResourceSet::~ResourceSet()
{
    qCDebug(lcResourceQt, "ResourceSet::%s(%d)", __FUNCTION__, identifier);
    for (int i = 0;i < NumberOfTypes;i++) {
        delete resourceSet[i];
    }
    if (resourceEngine != NULL) {
        qCDebug(lcResourceQt, "ResourceSet::%s(%d) - resourceEngine->disconnectFromManager()", __FUNCTION__, identifier);
        resourceEngine->disconnect(this);
        resourceEngine->disconnectFromManager();
    }
    qCDebug(lcResourceQt, "ResourceSet::%s(%d) - deleted!", __FUNCTION__, identifier);
}

bool ResourceSet::initialize()
{
    resourceEngine = new ResourceEngine(this);

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

    qCDebug(lcResourceQt) << QString("initializing resource engine...");
    if (!resourceEngine->initialize()) {
        return false;
    }
    qCDebug(lcResourceQt) << QString("resourceEngine->initialize() returned true");
    if (!resourceEngine->connectToManager()) {
        return false;
    }
    qCDebug(lcResourceQt, "ResourceSet is initialized engine:%d", resourceEngine->id());
    initialized = true;
    qCDebug(lcResourceQt, "**************** ResourceSet::%s().... %d", __FUNCTION__, __LINE__);
    return true;
}

void ResourceSet::addResourceObject(Resource *resource)
{
    qCDebug(lcResourceQt, "**************** ResourceSet::%s(%d).... %d", __FUNCTION__, this->id(), __LINE__);
    if (resource == NULL)
        return;
    qCDebug(lcResourceQt, "**************** ResourceSet::%s(%d).... %d", __FUNCTION__, this->id(), __LINE__);
    delete resourceSet[resource->type()];
    resourceSet[resource->type()] = resource;

    if ( resource->type() == AudioPlaybackType ) {

        qCDebug(lcResourceQt, "**************** ResourceSet::%s(%d).... %d", __FUNCTION__, this->id(), __LINE__);
        audioResource = static_cast<AudioResource *>(resource);
        QObject::connect(audioResource,
                          SIGNAL(audioPropertiesChanged(const QString &, quint32,
                                                         const QString &, const QString &)),
                          this,
                          SLOT(handleAudioPropertiesChanged(const QString &, quint32,
                                                             const QString &, const QString &)));
        if (!audioResource->audioGroupIsSet())
            audioResource->setAudioGroup(resourceClass);

        if (audioResource->streamTagIsSet() && (audioResource->processID() > 0)) {
            qCDebug(lcResourceQt) << QString("registering audio properties");
            registerAudioProperties();
        } else if (audioResource->audioGroupIsSet()) {
            qCDebug(lcResourceQt, "ResourceSet::%s().... %d registering audio proprerties later", __FUNCTION__, __LINE__);
            pendingAudioProperties = true;
        }

    } else if (resource->type() == VideoPlaybackType) {
        qCDebug(lcResourceQt, "**************** ResourceSet::%s(%d).... %d", __FUNCTION__, this->id(), __LINE__);
        videoResource = static_cast<VideoResource *>(resource);

        QObject::connect(videoResource,
                          SIGNAL(videoPropertiesChanged(quint32)),
                          this,
                          SLOT(handleVideoPropertiesChanged(quint32)));
        if (videoResource->processID() > 0) {
            qCDebug(lcResourceQt) << QString("registering video properties");
            registerVideoProperties();
        }
    }

    if (resourceEngine
        && (resourceEngine->isConnectedToManager() || resourceEngine->isConnectingToManager())) {
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
    case RearFlashlightType:
        resource = new RearFlashlightResource;
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
    if (type == AudioPlaybackType) {
        audioResource->disconnect();
        audioResource = NULL;
        pendingAudioProperties = false;
    }
    delete resourceSet[type];
    resourceSet[type] = NULL;

    if (resourceEngine
        && (resourceEngine->isConnectedToManager() || resourceEngine->isConnectingToManager())) {
        pendingUpdate = true;
    }

}

bool ResourceSet::contains(ResourceType type) const
{
    return ((type < NumberOfTypes) && (resourceSet[type] != NULL));
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
    if (!initialized) {
        qCDebug(lcResourceQt, "ResourceSet::%s().... initializing...", __FUNCTION__);
        return initialize();
    }
    if (!resourceEngine->isConnectedToManager()) {
        qCDebug(lcResourceQt, "ResourceSet::%s().... connecting...", __FUNCTION__);
        return resourceEngine->connectToManager();
    } else {
        qCDebug(lcResourceQt, "ResourceSet::%s(): already connected", __FUNCTION__);
    }

    return true;
}


bool ResourceSet::proceedIfImFirst( requestType theRequest )
{
    if (!ignoreQ) {
        requestQ.push_back(theRequest);
    } else {
        qCDebug(lcResourceQt, "ResourceSet::%s()...executing first request of %d.", __FUNCTION__, requestQ.size() );
        return true;
    }

    //Execute if this is the first request or the next is run from slot.
    if (requestQ.size() == 1 ) {
        if (!ignoreQ) { qCDebug(lcResourceQt, "ResourceSet::%s()...allowing only request directly.", __FUNCTION__); }
        return true;
    }

    if (requestQ.size() > 1) {
        qCDebug(lcResourceQt, "ResourceSet::%s()...queuing request %d.", __FUNCTION__, requestQ.size());

        switch (theRequest)
        {
        case Acquire:  qCDebug(lcResourceQt, "ResourceSet::%s()...queuing request:Acquire.", __FUNCTION__); break;
        case Update:   qCDebug(lcResourceQt, "ResourceSet::%s()...queuing request:Update.", __FUNCTION__);  break;
        case Release:  qCDebug(lcResourceQt, "ResourceSet::%s()...queuing request:Release.", __FUNCTION__); break;
        }
        return false;
    }

    Q_ASSERT_X(0, "proceedIfImFirst", "request queue can not be empty.");

    return false;
}


void ResourceSet::executeNextRequest()
{
    qCDebug(lcResourceQt) << Q_FUNC_INFO;

    if (requestQ.isEmpty()) {
        qCDebug(lcResourceQt) << Q_FUNC_INFO << QString("...the completed request is not present.");
        return;
    }

    requestQ.removeFirst(); //Remove completed request.

    if (requestQ.isEmpty()) {
        qCDebug(lcResourceQt) << Q_FUNC_INFO << QString("...last request acknowledged and removed.");
        return;
    }

    requestType nxtReq = requestQ.at(0);

    //Ensure that proceedIfimFirst() lets through.
    ignoreQ = true;
    //Having recursive mutexes, because it is taken again in proceedIfImFirst.
    qCDebug(lcResourceQt) << Q_FUNC_INFO << QString("...executing first request of %d.") << requestQ.size();

    switch (nxtReq)
    {
    case Acquire: qCDebug(lcResourceQt) << Q_FUNC_INFO << QString("...Acquire."); this->acquire();  break;
    case Update:  qCDebug(lcResourceQt) << Q_FUNC_INFO << QString("...Update."); this->update();   break;
    case Release: qCDebug(lcResourceQt) << Q_FUNC_INFO << QString("...Release."); this->release();  break;
    }

    ignoreQ = false;

    //Q_ASSERT_X(0, "executeNextRequest", "should not happen since requestQ.isEmpty() was false.");

}



bool ResourceSet::acquire()
{
    if (!initialized || !resourceEngine->isConnectedToManager()) {
        pendingAcquire = true;
        return initAndConnect();
    } else {
      /*  if (pendingUpdate)
        { //Connected and there are res.added.

            if ( !proceedIfImFirst( Update, Acquire ) ) return true;

            qCDebug(lcResourceQt) << Q_FUNC_INFO << QString(".... forcing update.");

            if (!resourceEngine->updateResources()) return false;

            if ( inAcquireMode ) return true;
        }*/

        if (!proceedIfImFirst(Acquire)) return true;

        qCDebug(lcResourceQt) << Q_FUNC_INFO << QString("... acquiring");
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
    qCDebug(lcResourceQt) << Q_FUNC_INFO << QString("... releasing...");
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

    if (!proceedIfImFirst(Update)) return true;

    qCDebug(lcResourceQt) << Q_FUNC_INFO << QString("... updating...");
    return resourceEngine->updateResources();
}

QString ResourceSet::applicationClass()
{
    return this->resourceClass;
}

bool ResourceSet::setAutoRelease()
{
    if (initialized)
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
    if (initialized)
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
    qCDebug(lcResourceQt, "**************** ResourceSet::%s().... %d", __FUNCTION__, __LINE__);
    if (resourceEngine->isConnectedToManager()) {
        qCDebug(lcResourceQt, "ResourceSet::%s() Connected to manager!", __FUNCTION__);
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
    } else { // assuming reconnecting
        qCDebug(lcResourceQt, "ResourceSet::%s() Reconnecting to manager...", __FUNCTION__);

        // first check if we have any acquired resources
        for (int i = 0; i < NumberOfTypes; i++) {
            if (resourceSet[i] != NULL) {
                if (resourceSet[i]->isGranted()) {

                    if (i == AudioPlaybackType) {
                        pendingAudioProperties = true;
                        qCDebug(lcResourceQt, "ResourceSet::%s() We have audio", __FUNCTION__);
                    }

                    if (i == VideoPlaybackType) {
                        pendingVideoProperties = true;
                        qCDebug(lcResourceQt, "ResourceSet::%s() We have video", __FUNCTION__);
                    }

                    qCDebug(lcResourceQt, "ResourceSet::%s() We have acquired resources. Re-acquire", __FUNCTION__);
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
        qCDebug(lcResourceQt, "%s(): initializing...", __FUNCTION__);
        pendingAudioProperties = true;
        initialize();
        return;
    } else if (resourceEngine->isConnectedToManager()) {
        qCDebug(lcResourceQt, "Registering new audio settings");
        //qCDebug(lcResourceQt,  "\taudio group: %s", audioResource->audioGroup().toStdString().c_str() );
        //qCDebug(lcResourceQt,  "\tPID: %d ", audioResource->processID() );
        //qCDebug(lcResourceQt,  "\taudio stream: %s:%s",  audioResource->streamTagName().toStdString().c_str(),
        //         audioResource->streamTagValue().toStdString().c_str() );

        if ((audioResource->processID() > 0) && audioResource->streamTagName() != "media.name") {
            qWarning() << "streamTagName should be 'media.name' it is '" << audioResource->streamTagName() << "'";
        }
        bool r = resourceEngine->registerAudioProperties(audioResource->audioGroup(),
                                                         audioResource->processID(),
                                                         audioResource->streamTagName(),
                                                         audioResource->streamTagValue());
        qCDebug(lcResourceQt, "resourceEngine->registerAudioProperties returned %s", r?"true":"false");

        pendingAudioProperties = false;
    } else { //if (!resourceEngine->isConnectedToManager() && !resourceEngine->isConnectingToManager()) {
        qCDebug(lcResourceQt, "%s(): Connecting to Manager...", __FUNCTION__);

        pendingAudioProperties = true;
        resourceEngine->connectToManager();
        return;
    }
}

void ResourceSet::registerVideoProperties()
{
    if (!initialized) {
        qCDebug(lcResourceQt, "%s(): initializing...", __FUNCTION__);
        pendingVideoProperties = true;
        initialize();
        return;
    } else if (resourceEngine->isConnectedToManager()) {

        qCDebug(lcResourceQt, "Registering new video settings:");
        qCDebug(lcResourceQt, "\tPID:%d", videoResource->processID() );

        if (videoResource->processID() < 2) {
            qWarning() << "processID should be > 1 '" << "'";
        }

        bool r = resourceEngine->registerVideoProperties( videoResource->processID() );

        qCDebug(lcResourceQt, "resourceEngine->registerVideoProperties returned %s", r?"true":"false");

        pendingVideoProperties = false;
    } else { //if (!resourceEngine->isConnectedToManager() && !resourceEngine->isConnectingToManager()) {
        qCDebug(lcResourceQt, "%s(): Connecting to Manager...", __FUNCTION__);

        pendingVideoProperties = true;
        resourceEngine->connectToManager();
        return;
    }
}

void ResourceSet::handleGranted(quint32 bitmaskOfGrantedResources)
{
    qCDebug(lcResourceQt, " ResourceSet::%s",__FUNCTION__);
    QList<ResourceType> optionalResources;
    qCDebug(lcResourceQt, "Acquired resources: 0x%04x", bitmaskOfGrantedResources);

    bool setChanged   = false;

    for (int i=0;i < NumberOfTypes; i++) {
        if (resourceSet[i] == NULL)
            continue;

        ResourceType type = (ResourceType)i;
        quint32 bitmask   = resourceTypeToLibresourceType(type);
        qCDebug(lcResourceQt, "Checking if resource 0x%04x is in the set", bitmask);

        if ((bitmask & bitmaskOfGrantedResources) == bitmask) {
            if (resourceSet[i]->isOptional()) {
                optionalResources << type;
            }
            if (!resourceSet[i]->isGranted())
                setChanged = true;

            resourceSet[i]->setGranted();
            qCDebug(lcResourceQt, "Resource 0x%04x is now granted", i);
        } else {
            if (resourceSet[i]->isGranted())
                setChanged = true;

            resourceSet[i]->unsetGranted();
            setChanged = true;
        }
    }

    //When we come to this slot bitmaskOfGrantedResources contains resources.
    if (alwaysReply || (!alwaysReply && setChanged)) {
        qCDebug(lcResourceQt, " ResourceSet::%s - emitting resourcesGranted(optionalResources) ",__FUNCTION__);
        emit resourcesGranted(optionalResources);
    }

    inAcquireMode = true;
    executeNextRequest();
}

void ResourceSet::handleReleased()
{
    for (int i=0;i < NumberOfTypes; i++) {
        if (resourceSet[i] != NULL) {
            resourceSet[i]->unsetGranted();
        }
    }

    if (alwaysReply || (!alwaysReply && inAcquireMode))
        emit resourcesReleased();

    qCDebug(lcResourceQt, "ResourceSet(%d) - resourcesReleased!", identifier);
    inAcquireMode = false;

    executeNextRequest();
    //emit resourcesReleased();
}

void ResourceSet::handleDeny()
{
    for (int i=0;i < NumberOfTypes; i++) {
        if (resourceSet[i] != NULL) {
            resourceSet[i]->unsetGranted();
        }
    }
    executeNextRequest();
    emit resourcesDenied();
}

void ResourceSet::handleResourcesLost(quint32 lostResourcesBitmask)
{
    for (int i=0;i < NumberOfTypes; i++) {
        quint32 bitmask = resourceTypeToLibresourceType((ResourceType)i);
        if ((bitmask & lostResourcesBitmask) == bitmask) {
            resourceSet[i]->unsetGranted();
            qCDebug(lcResourceQt, "Resource %04x is now lost", bitmask);
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
    qCDebug(lcResourceQt, "ResourceSet::%s().... %d", __FUNCTION__, __LINE__);

    if (resend) {
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

    qCDebug(lcResourceQt, "ResourceSet::%s()...about to exe next request....", __FUNCTION__);
    executeNextRequest();
}
