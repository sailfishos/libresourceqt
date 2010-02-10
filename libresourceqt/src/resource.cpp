#include <policy/resource.h>

using namespace ResourcePolicy;

Resource::Resource()
    :   optional(false), shared(false),
        identifier(0), granted(false)
{
    identifier = (quint32)this;
}

Resource::Resource(const Resource &other)
    :   optional(other.optional), shared(other.shared),
        identifier(other.identifier), granted(other.granted)
{
}

Resource::~Resource()
{
}

bool Resource::isOptional() const
{
    return optional;
}

void Resource::setOptional(bool resourceIsOptional)
{
    optional = resourceIsOptional;
}

bool Resource::isGranted() const
{
    return granted;
}

void Resource::setGranted()
{
    granted = true;
}

void Resource::unsetGranted()
{
    granted = false;
}


