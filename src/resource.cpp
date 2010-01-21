#include "resource.h"

Resource::Resource() : name(QString()), optional(false), shared(false)
{
    identifier = (quint32)this;
}

Resource::~Resource()
{
}

Resource::Resource(const Resource &other)
    : name(other.name), optional(other.optional),
      shared(other.shared), identifier(other.identifier)
{
}

Resource & Resource::operator=(const Resource &other)
{
    name = other.name;
    optional = other.optional;
    shared = other.shared;
    identifier = other.identifier;

    return *this;
}

QString Resource::type() const
{
    return name;
}

void Resource::setType(const QString type)
{
    name = type;
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
    if(name != other.name) {
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
