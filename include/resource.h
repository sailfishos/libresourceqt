#ifndef RESOURCE_H
#define RESOURCE_H

#include <QtCore>

namespace ResourcePolicy {
    enum ResourceType {
        AudioPlaybackType = 0,
        VideoPlaybackType,
        AudioRecorderType,
        VideoRecorderType,
        VibraType,
        LedsType,
        BacklightType,
        SystemButtonType,
        LockButtonType,
        ScaleButtonType,
        SnapButtonType,
        LensCoverType,
        NumberOfTypes
    };

    /**
    * This class is the super class for all resources. It represents a generic
    * \ref Resource. The type specific resource classes should be used.
    */
    class Resource
    {
    public:
        /**
         * Whether or not this resource is optional, in that it doesn't need to
         * be available for the set to be acquired.
         * \return true when this resource is optional.
         */
        bool isOptional() const;
        /**
         * Set the resource to be optional or mandatory.
         * \param resourceIsOptional This optional parameter defaults to true.
         * The default, true, results in the resource becoming optional. When it
         * is set to false the resource becomes mandatory.
         */
        void setOptional(bool resourceIsOptional=true);
        /**
         * Whether or not the resource to be shared. If it is shared then other
         * programs are allowed to share this resource.
         * \return true when this resource is shared.
         */
        bool isShared() const;
        /**
         * Sets the resource to be shared/private.
         * \param resourceIsShared This optional parameter defaults to true.
         * When it is set to true (de default) the resource is shared with
         * other programs. Setting it to false makes this resource exclusive.
         */
        void setShared(bool resourceIsShared=true);
        /**
         * Whether or not this resource has been granted.
         * \return true if this resource has been granted.
         */
        bool isGranted() const;
        void setGranted();
        void unsetGranted();

        virtual ResourceType type() const = 0;
        virtual Resource * clone() const = 0;
        virtual ~Resource();
    protected:
        Resource();
        Resource(const Resource &other);

        ResourceType resourceType;
        bool optional;
        bool shared;
        quint32 identifier;
        bool granted;
    };
}

#endif

