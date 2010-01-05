#include "resource.h"

Resource::Resource(enum ResourceClass requestedClass, quint16 requestedResources,
		   QObject *parent)
    : QObject(parent), resourceClass(requestedClass),
      resourceType(requestedResources), resourceState(UnknownState)
{
}

Resource::~Resource()
{
}

bool Resource::initialize(ResourceLibrary *library)
{
    resourceLibrary = library;
    return resourceLibrary->initialize();
}

bool Resource::connectToServer()
{
    return resourceLibrary->connectToServer();
}

quint16 Resource::resources() const
{
    return resourceType;
}

enum ResourceClass Resource::applicationClass() const
{
    return resourceClass;
}

bool Resource::hasResource(enum ResourceType resourceType) const
{
    if((this->resourceType & resourceType) == resourceType)
	return true;
    else
	return false;
}

bool Resource::hasExclusiveAccess() const
{
    if(resourceState == OwnedState)
	return true;
    else
	return false;
}

void Resource::handleStateChange(enum ResourceState newState)
{
    if(resourceState == UnknownState) {
	resourceState = newState;
	emit connectedToServer();
    }
    else {
	resourceState = newState;
	emit stateChanged(resourceState);
    }
}

bool Resource::requestExclusiveAccess()
{
    return false;
}
 
bool Resource::releaseExclusiveAccess()
{
    return false;
}

bool Resource::getExclusiveAccessState()
{
    return false;
}

