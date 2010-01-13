#include "resource-factory.h"
#include "libplayback-wrapper.h"

using namespace ResourceTypes;

ResourceFactory::ResourceFactory(QObject *parent)
    : QObject(parent), sessionBusConnection(QDBusConnection::sessionBus())
{
}

ResourceFactory::~ResourceFactory()
{
}

Resource * ResourceFactory::createResource(enum ResourceClass applicationClass,
					   quint16 requestedResources)
{
    qDebug("We %s connected to DBus",sessionBusConnection.isConnected()?"are":"are not");
    Resource *resource = new Resource(applicationClass, requestedResources, this);
    ResourceLibrary *libPlayback = new LibPlaybackWrapper(resource);

    bool resourceProperlyInitialized = resource->initialize(libPlayback);
    if(resourceProperlyInitialized) {
	return resource;
    }
    else {
	qDebug("Failed to initialize Resource. Returning NULL!");
	delete resource;
	return NULL;
    }
}
