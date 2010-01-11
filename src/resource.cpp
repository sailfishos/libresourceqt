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

bool Resource::isReserved() const
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

void Resource::emitReservable()
{
    return;
}

bool Resource::reserve()
{
    return false;
}
 
bool Resource::release()
{
    return false;
}

bool Resource::requestState()
{
    return false;
}

bool Resource::setMute()
{
    return false;
}

bool Resource::unsetMute()
{
    return false;
}

bool Resource::requestMute()
{
    return false;
}

bool Resource::setPrivacyOverride()
{
    return false;
}

bool Resource::unsetPrivacyOverride()
{
    return false;
}

bool Resource::requestPrivacyOverride()
{
    return false;
}

bool Resource::setBluetoothOverride()
{
    return false;
}

bool Resource::unsetBluetoothOverride()
{
    return false;
}

bool Resource::requestBluetoothOverride()
{
    return false;
}

bool Resource::setPid(quint32 pid)
{
    return false;
}

bool Resource::setStreamName(const QString & name)
{
    return false;
}

