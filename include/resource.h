#ifndef RESOURCE_H
#define RESOURCE_H

#include <QtCore>


namespace ResourcePolicy {
   /**
    * This enum defines the different resources that an application can use.
    */
   enum ResourceType {
	   InvalidResource = 0,
	   AudioPlaybackResource,
	   VideoPlaybackResource,
	   AudioRecorderResource,
	   VideoRecorderResource,
	   ResourceGuard
   };

   /**
    * This class represents a resource which can be acquired. The main use of
    * Resources is to use then in a ResourceSet.
    * 
    * Other Resource types can be added later, these reflect the resources found
    * in the Policy configuration.
    * 
    * \b Examples:
    * This example shows how to make a resource which represents an optional
    * audio requirement.
    * \code
    * Resource audioResource(AudioPlaybackResource);
    * audioResource->setOptional();
    * \endcode
    * This example shows how to create the resources used by a video player
    * \code
    * Resource audioResource(AudioPlaybackResource);
    * Resource videoResource(VideoPlaybackResource);
    * \endcode
    */
   class Resource
   {
  public:
      /**
       * Constructor. Creates a new identifier. The new identifier defaults to
       * not optional, and not shared. The default type is undefined and
       * \ref setType() needs to be called to set the type. Optionally a
       * resource type may be given to the constructor, to avoid having to
       * call the setType() method.
       */
      Resource(enum ResourceType type=InvalidResource);
      /**
       * Copy constructor. Copies also the identifier.
       * \param other The source to copy from.
       */
      Resource(const Resource &other);
      /**
       * Assignement operator. Works like the copy constructor.
       * \param other The source to assign from.
       */
      Resource &operator=(const Resource &other);
      //! Destructor.
      ~Resource();

      /**
       * Returns the type of the resource.
       * \return the type of the resource.
       */
      ResourceType type() const;
      /**
       * Sets the resource type.
       * \param type The type to set to.
       * \return true if the type is valid.
       */
      bool setType(enum ResourceType type);
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
       * Compares two resources and returns true if both are of the same \b type.
       * \param other The resrouce to compare to.
       * \return true if the types of both resources are of the same type.
       */
      bool operator==(const Resource &other) const;
  private:
      ResourceType resourceType;
      bool optional;
      bool shared;
      quint32 identifier;
   };

}

#endif
