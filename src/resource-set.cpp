#include "resource-set.h"


ResourcePolicy::ResourceSet::ResourceSet(const QString &applicationClass)
    : applicationClass(applicationClass), resourceSet()
{
    identifier = (quint32)this;
}

ResourcePolicy::ResourceSet::~ResourceSet()
{
}

void ResourcePolicy::ResourceSet::addResource(const ResourcePolicy::Resource &resource)
{
    resourceSet.insert(resource);
}

void ResourcePolicy::ResourceSet::addResources(const QSet<ResourcePolicy::Resource> &resources)
{
    resourceSet.unite(resources);
}

void ResourcePolicy::ResourceSet::setResources(const QSet<ResourcePolicy::Resource> &resources)
{
    resourceSet.clear();
    resourceSet = resources;
}

bool ResourcePolicy::ResourceSet::contains(const ResourcePolicy::Resource &resource) const
{
    return resourceSet.contains(resource);
}

bool ResourcePolicy::ResourceSet::contains(const QSet<ResourcePolicy::Resource> &resources) const
{
    return resourceSet.contains(resources);
}

quint32 ResourcePolicy::ResourceSet::id() const
{
    return identifier;
}

QSet<ResourcePolicy::Resource> ResourcePolicy::ResourceSet::resources()
{
    return resourceSet;
}
