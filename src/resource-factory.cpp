#include "resource-factory.h"
#include "libplayback-wrapper.h"

ResourceFactory::ResourceFactory(QObject *parent)
    : QObject(parent), sessionBusConnection(QDBusConnection::sessionBus())
{
}

ResourceFactory::~ResourceFactory()
{
}

Resource * ResourceFactory::createResource(enum ResourceClass type, quint16 usageFlags)
{
    Resource *resource = new Resource(type, usageFlags, this);
    ResourceLibrary *libPlayback = new LibPlaybackWrapper(resource);

    bool resourceProperlyInitialized = resource->initialize(libPlayback);
    if(resourceProperlyInitialized) {
	return resource;
    }
    else {
	delete resource;
	return NULL;
    }
}
