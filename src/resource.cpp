#include "resource.h"

using namespace ResourcePolicy;

Resource::Resource(enum ResourceType type)
    : resourceType(type), optional(false), shared(false)
{
}

Resource::~Resource()
{
}

Resource::Resource(const Resource &other)
    : resourceType(other.resourceType), optional(other.optional),
      shared(other.shared)
{
}

Resource & Resource::operator=(const Resource &other)
{
    resourceType = other.resourceType;
    optional = other.optional;
    shared = other.shared;

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

bool Resource::operator==(const Resource &other) const
{
    if(resourceType != other.resourceType) {
	return false;
    }
    return true;
}
