#include "resource.h"

using namespace ResourcePolicy;

Resource::Resource() : resourceType(InvalidResource), optional(false), shared(false)
{
    identifier = (quint32)this;
}

Resource::~Resource()
{
}

Resource::Resource(const Resource &other)
    : resourceType(other.resourceType), optional(other.optional),
      shared(other.shared), identifier(other.identifier)
{
}

Resource & Resource::operator=(const Resource &other)
{
    resourceType = other.resourceType;
    optional = other.optional;
    shared = other.shared;
    identifier = other.identifier;

    return *this;
}

ResourceType Resource::type() const
{
    return resourceType;
}

bool Resource::setType(ResourceType type)
{
    if(type != InvalidResource) {
	resourceType = type;
	return true;
    }
    else {
	return false;
    }
}

bool Resource::isOptional() const
{
    return optional;
}

void Resource::setOptional(bool resourceIsOptional)
{
    optional = resourceIsOptional;
}

bool Resource::isShared() const
{
    return shared;
}

void Resource::setShared(bool shared)
{
    this->shared = shared;
}

quint32 Resource::id() const
{
    return identifier;
}

void Resource::setId(quint32 newId)
{
    identifier = newId;
}

bool Resource::operator==(const Resource &other)
{
    if(resourceType != other.resourceType) {
	return false;
    }
    if((identifier != other.identifier) or
       (shared != other.shared) or 
       (optional != other.optional))
    {
	return false;
    }
    return true;
}
