#include <policy/resource-set.h>
using namespace ResourcePolicy;


ResourceSet::ResourceSet(const QString &applicationClass, QObject * parent)
    : QObject(parent), resourceClass(applicationClass)
{
    identifier = (quint32)this;
    memset(resourceSet, 0, sizeof(QPointer<Resource *>)*NumberOfTypes);
}

ResourceSet::~ResourceSet()
{
    for(int i=0;i<NumberOfTypes;i++) {
        delete resourceSet[i];
    }
}

void ResourceSet::addResource(const Resource *resource)
{
    resourceSet[resource->type()] = resource->clone();
}

void ResourceSet::addResources(const QList<Resource *>resources)
{
    for(int i=0; i < resources.size(); i++) {
        addResource(resources.at(i));
    }
}

void ResourceSet::delResource(ResourceType type)
{
    if(contains(type)) {
        delete resourceSet[type];
        resourceSet[type] = NULL;
    }
}

bool ResourceSet::contains(ResourceType type) const
{
    if((type < NumberOfTypes) && (resourceSet[type] != NULL))
        return true;
    else
        return false;
}

bool ResourceSet::contains(const QList<ResourceType> &types) const
{
    bool containsAll=true;
    int i=0;
    do {
        containsAll = contains(types.at(i));
        i++;
    } while((i < types.size()) && containsAll);
    return containsAll;
}

quint32 ResourceSet::id() const
{
    return identifier;
}

QList<Resource *> ResourceSet::resources() const
{
    QList<Resource *> listOfResources;
    for(int i=0; i < NumberOfTypes; i++) {
        if(resourceSet[i] != NULL) {
            Resource *clone = resourceSet[i]->clone();
            listOfResources.append(clone);
        }
    }
    return listOfResources;
}

Resource * ResourceSet::resource(ResourceType type) const
{
    if(contains(type))
        return resourceSet[type]->clone();
    else
        return NULL;
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

QString ResourceSet::applicationClass()
{
    return this->resourceClass;
}
