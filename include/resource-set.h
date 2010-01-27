#ifndef RESOURCE_SET_H
#define RESOURCE_SET_H

#include "resource.h"
#include <QString>
#include <QObject>
#include <QVector>
#include <QList>

/**
 * \mainpage Resource Policy Library
 * 
 * \section intro_section Introduction
 * 
 * This library is used to request resources from the Polict Resource Manager.
 * To use this library two classes are provided: \ref ResourcePolicy::Resource and
 * \ref ResourcePolicy::ResourceSet.
 * 
 * \section library_use_section Library Usage
 * 
 * To use the Resource Policy Library, you first need to create a set of
 * \ref Resource objects like this (given as an example of what a media player
 * might want/need):
 * \code
 * ResourcePolicy::Resource audioResource(AudioPlaybackResource);
 * ResourcePolicy::Resource videoResource(VideoPlaybackResource);
 * videoResource.setOptional();
 * \endcode
 * Then you need to create a \ref ResourcePolicy::ResourceSet like this:
 * \code
 * ResourcePolicy::ResourceSet *resources = new ResourcePolicy::ResourceSet("player");
 * resources.addResource(audioResource);
 * resources.addResource(videoResource);
 * resources->initialize();
 * QObject::connect(resources, SIGNAL(connectedToManager), this, SLOT(connectedHandler()));
 * resources->connectToManager();
 * \endcode
 * Then the when you want to acquire the \ref ResourcePolicy::ResourceSet you simply use the
 * \ref acquire() method of the \ref ResourceSet object, like this:
 * \code
 * QObject::connect(resources, SIGNAL(resourcesAcquired),
 *                  this, SLOT(acquireOkHandler(QList<ResourcePolicy::Resource>)));
 * QObject::connect(resources, SIGNAL(resourcesDenied), this, SLOT(acquireDeniedHandler()));
 * resources->acquire();
 * \endcode
 */

/**
 * The Namespace for Resource Policy.
 */
namespace ResourcePolicy
{
   /**
    * The resourceSet repesents a set of attributes. Each set can only contain
    * a single Resource of a given type. That is one AudioPlaybackResource, etc.
    *
    * Internally the set is stored as a QVector of \ref Resource objects.
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
       * \param parent The optional parent of of this class.
       */
      ResourceSet(const QString &applicationClass, QObject *parent=NULL);
      /**
       * The destructor
       */
      ~ResourceSet();
      /**
       * Initializes the underlaying communications library.
       * \return true if initialization is successful.
       */
      bool initialize();
      
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

      /**
       * Checks if the \ref ResourceSet contains the given \ref Resource
       * \param resource The Resource to look for
       * \return true if the \ref Resource is defined in this \ref ResourceSet
       */
      bool contains(const Resource &resource) const;

      /**
       * Checks if the \ref ResourceSet contains all given resources.
       * \param resources A list of resources to check for
       * \return true if \b all given resources are defined in the ResourceSet.
       */
      bool contains(const QList<Resource> &resources) const;

      quint32 id() const;

      /**
       * Connects to the Resource Policy Manager. The connected() signal is sent
       * when the connection has been established. Acquiring the ResourceSet can
       * be attempted at any time after this, but not before.
       * \return true if the connection request was successfully sent.
       * \param reconnectOnDisconnect. Set to true to automatically reconnect on
       * lost connection to the Policy Resource Manager. This optional parameter
       * defaults to true.
       */
      bool connectToManager(bool reconnectOnDisconnect);
      /**
       * Disconnects from the Resource Policy Manager. Further attempts at acquiring
       * the \ref ResourceSet will fail.
       */
      void disconnectFromManager();
      /**
       * Checks whether the ResourceSet is connected or not.
       */
      bool isConnectedToManager();
      /**
       * Attempt to acquire the ResourceSet. The response is returned as the
       * resourcesAcquired() signal.
       */
      bool acquire();
      /**
       * Release the acquired resources.
       */
      bool release();
      /**
       * Commit changes to the \ref ResourceSet to the Manager.
       */
      bool update();

  signals:
      /**
       * This signal is emited when the Resource Policy Manager notifies that
       * the given resources have become available.
       * \param availableResources A list of available resources.
       */
      void resourcesBecameAvailable(QList<Resource> availableResources);
      /**
       * This signal is emited as a response to the acquire() request.
       * \param grantedResources The list of granted resources.
       */
      void resourcesAcquired(QList<Resource> grantedResources);
      /**
       * This signal is emited as a response to the acquire() request, in the
       * case where we are not granted any requests.
       */
      void resourcesDenied();
      /**
       * This signal is emited when some other program with a higher priority
       * superseeds us, and as a result we loose our resources.
       */
      void lostResources();
      /**
       * This signal is emited when we have successfully connected to the manager.
       */
      void connectedToManager();
      /**
       * This signal is emited when we loose the connection to the manager.
       * A reconnect is automatically attempted.
       */
      void disconnectedFromManager();
      

  private:
      quint32 identifier;
      const QString applicationClass;
      QVector<Resource> resourceSet;
   };
}

#endif
