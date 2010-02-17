#ifndef RESOURCE_SET_H
#define RESOURCE_SET_H

#include <QString>
#include <QObject>
#include <QVector>
#include <QList>
#include <policy/resources.h>
#include <policy/audio-resource.h>

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
 * To use the Resource Policy Library, you first need to create a number of
 * \ref Resource objects like this (given as an example of what a media player
 * might want/need):
 * \code
 * ResourcePolicy::AudioResource *audioResource = new ResourcePolicy::AudioResource();
 * ResourcePolicy::VideoResource *audioResource = new ResourcePolicy::VideoResource();
 * videoResource->setOptional();
 * \endcode
 * Then you need to create a \ref ResourcePolicy::ResourceSet like this:
 * \code
 * ResourcePolicy::ResourceSet *resources = new ResourcePolicy::ResourceSet("player");
 * resources->addResource(audioResource);
 * resources->addResource(videoResource);
 * \endcode
 * The resource set now has control over the Resource object pointers. You can
 * drop them, but should NOT delete them. Instead call the ResourcePolicy::ResourceSet::deleteResource() 
 * method. Then when you want to acquire the \ref ResourcePolicy::ResourceSet
 * you simply use the \ref ResourcePolicy::ResourceSetacquire() method, like this:
 * \code
 * QObject::connect(resources, SIGNAL(resourcesAcquired()),
 *                  this, SLOT(acquireOkHandler(QList<ResourcePolicy::Resource>)));
 * QObject::connect(resources, SIGNAL(resourcesDenied()), this, SLOT(acquireDeniedHandler()));
 * resources->acquire();
 * \endcode
 * You should also connect to the ResourcePolicy::ResourceSet::lostResources() signal like this:
 * \code
 * QObject::connect(resources, SIGNAL(lostResources()),
 *                  this, SLOT(lostResources()));
 * \endcode
 * This signal tells you when you should stop using the resources you have asked for.
 * So it is important that you connect to it.
 */

/**
 * The Namespace for Resource Policy.
 */
namespace ResourcePolicy
{
class ResourceEngine;
/**
 * The resourceSet repesents a set of attributes. Each set can only contain
 * a single Resource of a given type. That is one AudioPlaybackResource, etc.
 *
 * Internally the set is stored as a QVector of \ref Resource objects.
 */
class ResourceSet: public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ResourceSet)
public:
    /**
     * The constructor.
     * \param applicationClass This parameter defines the application class.
     * The application class is used to determine the priority order of the
     * application.
     * \param parent The optional parent of of this class.
     */
    ResourceSet(const QString &applicationClass, QObject *parent = NULL);
    /**
     * The destructor
     */
    ~ResourceSet();

    /**
     * This method adds a resource to the set. A set contains only a single
     * instance of a given resource. If the ResourceSet already contains a
     * resource of the given type it will be overridden.
     * \param resource The resource to add to the set. A copy of this object
     * is stored in the Set.
     */
    void addResource(Resource *resource);
    /**
     * This method adds all resources in the list to the set.
     * A set contains only a single instance of a given resource. If the
     * ResourceSet already contains a resource of the given type it will be
     * overridden.
     * \param resources The list of resources to add to the set. These will
     * be copied.
     */
    void addResources(const QList<Resource *>resources);
    /**
     * This method removes the resource of the given type
     * \param type The type of the resource to remove from the set.
     */
    void deleteResource(ResourceType type);

    /**
     * This method returns a list of all resource in the set.
     * \return a QList of all resources in the set.
     */
    QList<Resource *> resources() const;
    /**
     * This method returns a const pointer to a resource of a specific type.
     * \type The type of resource we are interested in.
     * \return a pointer to the Resource if it is defined NULL otherwise.
     */
    Resource * resource(ResourceType type) const;
    /**
     * Checks if the \ref ResourceSet contains the given \ref Resource
     * \param type The Resource to look for
     * \return true if the \ref Resource is defined in this \ref ResourceSet
     */
    bool contains(ResourceType type) const;

    /**
     * Checks if the \ref ResourceSet contains all given resources.
     * \param types A list of resources to check for
     * \return true if \b all given resources are defined in the ResourceSet.
     */
    bool contains(const QList<ResourceType> &types) const;

    /**
     * Returns the unique identifier for this ResourceSet.
     * @return the unique identifier for this ResourceSet.
     */
    quint32 id() const;

    /**
     * Returns the registered application class (given in the constructor).
     */
    QString applicationClass();

    /**
     * Try to acquire the \ref ResourceSet. The resourcesGranted() or
     * resourcesDenied() signal will be emited depending on whether the
     * requested resources could be acquired or not.
     */
    bool acquire();
    /**
     * Release the acquired resources.
     */
    bool release();
    /**
     * Commit changes to the \ref ResourceSet. Remember to call update()
     * after adding and/or removing resources.
     */
    bool update();

    /**
     * Stes the auto-release. When loosing the resources doue to another
     * application with a higher priority the default is that we automatically
     * re-gain our resources without having to re-request them. However if
     * the AutoRelease is set we release the resources and need to re-acquire
     * them, when the pre-emting application releases it s resources.
     * This feature is by default disabled.
     * Remember to call update() when changing this.
     */
    void setAutoRelease();
    /**
     * Sets that the resourcesGranted() signal is emited even if we already
     * have the requested resources granted. By default this feature is off.
     * This flag should be set once only before finalize is called, and cannot be unset.
     */
    void setAlwaysReply();

signals:
    /**
     * This signal is emited when the Resource Policy Manager notifies that
     * the given resources have become available.
     * \param availableResources A list of available resources. The list of
     * available resources contains only available resource which we have in the set.
     */
    void resourcesBecameAvailable(const QList<ResourceType> &availableResources);
    /**
     * This signal is emited as a response to the acquire() request.
     * \param grantedOptionalResources The list of granted optional resources.
     * All the mandatory resources have also been aquired.
     */
    void resourcesGranted(const QList<ResourceType> &grantedOptionalResources);
    /**
     * This signal is emited as a response to the acquire() request, in the
     * case where one or more of the mandatroy resources were not availalble.
     */
    void resourcesDenied();
    /**
     * This signal is emited as a response to the release() request.
     */
    void resourcesReleased();
    /**
     * This signal is emited when some other program with a higher priority
     * superseeds us, and as a result we loose (some of) our resources.
     * It is very important to connect to this signal as it is signaling when
     * the acquired resources shouldn't be used anymore.
     */
    void lostResources();

private:

    bool initialize();

    quint32 identifier;
    const QString resourceClass;
    Resource* resourceSet[NumberOfTypes];
    ResourceEngine *resourceEngine;
    AudioResource *audioResource;
    bool autoRelease;
    bool alwaysReply;
    bool initialized;
    bool pendingAcquire;
    bool pendingUpdate;
    bool pendingAudioGroup;
    bool pendingAudioStream;
    bool pendingAudioPid;
    
private slots:
    void connectedHandler();
    void handleGranted(quint32);
    void handleDeny();
    void handleReleased();
    void handleResourcesLost(quint32);
    void handleResourcesBecameAvailable(quint32);

    void handleAudioPidChange(quint32 newPid);
    void handleAudioGroupChange(const QString &newGroup);
    void handleAudioStreamTagChanged(const QString &newGroup);
};
}

#endif

