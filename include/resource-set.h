#ifndef RESOURCE_SET_H
#define RESOURCE_SET_H

#include "resource.h"
#include <QString>
#include <QObject>
#include <QVector>
#include <QList>

namespace ResourcePolicy
{
   /**
    * The resourceSet repesents a set of attributes. Each set can only contain
    * a single Resource of a given type. That is one AudioPlaybackResource, etc.
    *
    * Internally the set is stored as a QVector of \ref Resource objects.
    *
    * \b Examples:
    * This example shows how to create a resourceSet used by a video player
    * \code
    * Resource audioResource(AudioPlaybackResource);
    * Resource videoResource(VideoPlaybackResource);
    * ResourceSet myResourceSet("player");
    * myResourceSet.addResource(audioResource);
    * myResourceSet.addResource(videoResource);
    * \endcode
    */
   class ResourceSet: public QObject
   {
      Q_OBJECT
      Q_DISABLE_COPY( ResourceSet )
  public:
      /**
       * The constructor.
       * \param applicationClass This parameter defines the application class.
       * The application class is used to determine the priority order of the
       * application.
       */
      ResourceSet(const QString &applicationClass);
      /**
       * The destructor
       */
      ~ResourceSet();

      /**
       * This method adds a resource to the set. A set contains only a single
       * instance of a given resource. If the ResourceSet already contains a
       * resource of the given type it will be overridden.
       * \param resource The resource to add to the set.
       */
      void addResource(const Resource &resource);
      /**
       * This method adds (and replaces) the list of resource to the set.
       * \param resources The list of resources to add to the set.
       */
      void addResources(const QList<Resource> &resources);
      /**
       * This method clears the old resources and replaces the set with the
       * resources from the given list.
       * \param resources The list of resources that the set should contain.
       */
      void setResources(const QList<Resource> &resources);

      /**
       * This method returns a list of all resource in the set.
       * \return a QList of all resources in the set.
       */
      QList<Resource> resources();

      bool contains(const Resource &resource) const;
      bool contains(const QList<Resource> &resources) const;

      quint32 id() const;
  signals:
      void resourcesBecameAvailable(QList<Resource> availableResources);
      void resourcesAcquired(QList<Resource> grantedResources);
      void resourcesDenied();
      void lostResources();

  private:
      quint32 identifier;
      const QString applicationClass;
      QVector<Resource> resourceSet;
   };
}

#endif
