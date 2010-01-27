#include "resource-set.h"
using namespace ResourcePolicy;


ResourceSet::ResourceSet(const QString &applicationClass, QObject * parent)
    : QObject(parent), applicationClass(applicationClass),
      resourceSet(ResourceGuard)
{
    identifier = (quint32)this;
}

ResourceSet::~ResourceSet()
{
}

void ResourceSet::addResource(const Resource &resource)
{
    resourceSet.insert(resource.type(), resource);
}

void ResourceSet::addResources(const QList<Resource> &resources)
{
    for(int i=0; i < resources.size(); i++) {
	addResource(resources.at(i));
    }
}

void ResourceSet::setResources(const QList<Resource> &resources)
{
    Resource invalidResource;
    resourceSet.clear();
    resourceSet.resize(ResourceGuard);
    for(int i=0; i < resources.size(); i++) {
	addResource(resources.at(i));
    }
}

bool ResourceSet::contains(const Resource &resource) const
{
    if(resourceSet.at(resource.type()) == resource)
	return true;
    else
	return false;
}

bool ResourceSet::contains(const QList<Resource> &resources) const
{
    bool containsAll=false;
    for(int i=0; i<resources.size(); i++) {
	containsAll = contains(resources.at(i));
	if(!containsAll) {
	    break;
	}
    }
    return containsAll;
}

quint32 ResourceSet::id() const
{
    return identifier;
}

QList<Resource> ResourceSet::resources()
{
    QList<Resource> listOfResources;
    for(int i=0; i < resourceSet.size(); i++) {
	if(resourceSet.at(i).type() != InvalidResource) {
	    listOfResources.append(resourceSet.at(i));
	}
    }
    return listOfResources;
}

bool ResourceSet::connectToManager(bool reconnectOnDisconnect)
{
    return false;
}

void ResourceSet::disconnectFromManager()
{
}

bool ResourceSet::isConnectedToManager()
{
    return false;
}

bool ResourceSet::acquire()
{
    return false;
}

bool ResourceSet::release()
{
    return false;
}

bool ResourceSet::update()
{
    return false;
}
