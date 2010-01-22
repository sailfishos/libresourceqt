#ifndef RESOURCE_H
#define RESOURCE_H

#include <QtCore>


namespace ResourcePolicy {
   /**
    * This enum defines the different resources that an application can use.
    */
   enum ResourceType {
	   InvalidResource = 0,
	   AudioPlaybackResource = 0x01,
	   VideoPlaybackResource = 0x02,
	   AudioRecorderResource = 0x04,
	   VideoRecorderResource = 0x08
   };

   /**
    * This class represents a resource which can be acquired. The main use of
    * Resources is to use then in a ResourceSet.
    * 
    * Other Resource types can be added later, these reflect the resources found
    * in the Policy configuration. 
    */
   class Resource
   {
  public:
      Resource(const Resource &other);
      Resource();
      Resource &operator=(const Resource &other);

      ~Resource();

      ResourceType type() const;
      bool setType(enum ResourceType type);
      bool isOptional() const;
      void setOptional(bool resourceIsOptional=true);
      bool isShared() const;
      void setShared(bool shared=true);
      quint32 id() const;
      void setId(quint32 newId);

      bool operator==(const Resource &other) const;
  private:
      ResourceType resourceType;
      bool optional;
      bool shared;
      quint32 identifier;
   };
   
   uint qHash(const ResourcePolicy::Resource &key);
}

#endif
