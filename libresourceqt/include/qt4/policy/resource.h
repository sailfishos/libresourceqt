#ifndef RESOURCE_H
#define RESOURCE_H

#include <QtCore>

namespace ResourcePolicy
{
/**
  * This enumeration represents the resources which can be reserved.
  * \see ResourcePolicy::ResourceSet::AddResource() for info on how to reserve
  * resources.
  */
enum ResourceType {
    AudioPlaybackType = 0, ///< For audio playback
    VideoPlaybackType,     ///< For video playback
    AudioRecorderType,     ///< For audio recording (using of the microphone)
    VideoRecorderType,     ///< For video recording (using the camera)
    VibraType,             ///< For Vibra
    LedsType,              ///< For LEDs
    BacklightType,         ///< For the backlight (of the display)
    SystemButtonType,      ///< For the system (power) button
    LockButtonType,        ///< For the lock button
    ScaleButtonType,       ///< The scale (zoom) button
    SnapButtonType,        ///< Use this if you are a camera application
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
    void setOptional(bool resourceIsOptional = true);
    /**
     * Whether or not the resource to be shared. If it is shared then other
     * programs are allowed to share this resource.
     * \return true when this resource is shared.
     */
    bool isGranted() const;

    /**
     * Use this method to check for the type of Resource
     * \return The ResourceType associated to this resource
     */
    virtual ResourceType type() const = 0;
    virtual ~Resource();
protected:
    Resource();
    Resource(const Resource &other);

    ResourceType resourceType;
    bool optional;
    quint32 identifier;
private:
    void setGranted();
    void unsetGranted();
    bool granted;

};
}

#endif

