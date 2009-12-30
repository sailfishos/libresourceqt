#include "resource.h"

Resource::Resource(enum ResourceClass type, quint16 resourceFlags, QObject *parent)
    : QObject(parent), resourceClass(type), flags(resourceFlags)
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


quint16 Resource::getResourceFlags()
{
    return flags;
}

enum ResourceClass Resource::getResourceClass()
{
    return resourceClass;
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
