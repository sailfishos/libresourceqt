/*************************************************************************
This file is part of libresourceqt

Copyright (C) 2010 Nokia Corporation.

This library is free software; you can redistribute
it and/or modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation
version 2.1 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
USA.
*************************************************************************/
/**
 * @file resource-set.h
 * @brief Declaration of ResourcePolicy::ResourceSet
 *
 * @copyright Copyright (C) 2010 Nokia Corporation.
 * @author Wolf Bergenheim
 * \par License
 * @license LGPL
 * This file is part of libresourceqt
 * \par
 * Copyright (C) 2010 Nokia Corporation.
 * \par
 * This library is free software; you can redistribute
 * it and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation
 * version 2.1 of the License.
 * \par
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * \par
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
 * USA.
 */

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
 * This library is used to request resources from the Policy Resource Manager.
 * To use this library two classes are provided: \ref ResourcePolicy::Resource and
 * \ref ResourcePolicy::ResourceSet.
 *
 * \section library_use_section Library Usage
 *
 * To use the Resource Policy Library, you first need to create the
 * \ref ResourcePolicy::ResourceSet like this:
 * \code
 * ResourcePolicy::ResourceSet *resources = new ResourcePolicy::ResourceSet("player");
 * \endcode
 * Then to add resources to the set use the ResourceSet::addResource() method to add
 * resources to the ResourcePolicy::ResourceSet. Like this:
 * \code
 * resources->addResource(AudioPlaybackType);
 * resources->addResource(VideoPlaybackType);
 * \endcode
 * If you want to pre-populate the AudioResource with the audio group (it is a good idea)
 * and other audio parameters you can create an audio object yourself and then
 * give that to the ResourcePolicy::ResourceSet. Note that you should NOT free this object.
 * The ResourcePolicy::ResourceSet takes ownership of this pointer.
 * \code
 * ResourcePolicy::AudioResource *audioResource = new ResourcePolicy::AudioResource("player");
 * resources->addResourceObject(audioResource);
 * \endcode
 * Calling the ResourcePolicy::ResourceSet::deleteResource() method will remove
 * and delete the object. Then when you want to acquire the ResourcePolicy::ResourceSet
 * you simply use the ResourcePolicy::ResourceSet::acquire() method, like this:
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
 *
 * To modify the properties of the resources you can use the ResourcePolicy::ResourceSet::resource() method.
 *
 * \section see_devel_doc See Also
 * For a more detailed guide see the
 * <a href="https://projects.maemo.org/mediawiki/index.php/Maemo_Developer_Guide/Developing_Harmattan_applications/Application_policy_guidelines">Application policy guidelines</a>.
 */

/**
 * \brief The Namespace for Resource Policy.
 * All Resource Policy Qt APIs reside under the ResourcePolicy namespace.
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
     * Alternative constructor with additional parameters for setting
     * alwaysReply and autoRelease.
     * \param applicationClass This parameter defines the application class.
     * The application class is used to determine the priority order of the
     * application.
     * \param parent The parent of this class.
     * \param alwaysReply see setAlwaysReply()
     * \param autoRelease see setAutoRelease()
     */
    ResourceSet(const QString &applicationClass, QObject *parent,
                bool alwaysReply, bool autoRelease);
    /**
     * Alternative backwards-compatible constructor.
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
     * \param resourceType The resource to add to the set. A copy of this object
     * is stored in the Set.
     */
    bool addResource(ResourceType resourceType);

    /**
    * This method adds a resource to the set. A set contains only a single
    * instance of a given resource. If the ResourceSet already contains a
    * resource of the given type it will be overridden.
    * \param resource The resource to add to the set.
    * The ResourcePolicy::ResourseSet takes ownership of the pointer. Do NOT free!
    */
    void addResourceObject(Resource *resource);

    /**
     * This method removes the resource of the given type
     * \param type The type of the resource to remove from the set.
     */
    void deleteResource(ResourceType type);

    /**
     * This method returns a list of all resource in the set.
     * \return a QList of all resources in the set.
     */    QList<Resource *> resources() const;
    /**
     * This method returns a const pointer to a resource of a specific type.
     * \param type The type of resource we are interested in.
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
     * Commit changes to the \ref ResourcePolicy::ResourceSet. Remember to call update()
     * after adding and/or removing resources.
     */
    bool update();

    /**
     * Sets the auto-release. When loosing the resources due to another
     * application with a higher priority the default is that we automatically
     * re-gain our resources without having to re-request them. However if
     * the AutoRelease is set we release the resources and need to re-acquire
     * them, when the pre-emting application releases its resources.
     *
     * This feature is by default disabled.
     *
     * This flag should be set once only before calling anything else
     * (excluding setAlwaysReply()), and cannot be unset.
     */
    bool setAutoRelease();
    /**
     * Check whether we have setAutoRelease().
     * \return true if auto-release is ennabled.
     */
    bool willAutoRelease();
    /**
     * Sets that the resourcesGranted() signal is emited even if we already
     * have the requested resources granted. By default this feature is off.
     *
     * This flag should be set once only before calling anything else
     * (excluding setAutoRelease()), and cannot be unset.
     */
    bool setAlwaysReply();
    /**
     * Check whether the always-get-reply flag has been set.
     * \return true if we will always get a reply (even if there is no change).
     */
    bool alwaysGetReply();

signals:
    /**
     * This signal is emited when the Resource Policy Manager notifies that
     * the given resources have become available.
     * \param availableResources A list of available resources. The list of
     * available resources contains only available resource which we have in the set.
     */
    void resourcesBecameAvailable(const QList<ResourcePolicy::ResourceType> &availableResources);
    /**
     * This signal is emitted as a response to the acquire() request.
     * \param grantedOptionalResources The list of granted optional resources.
     * All the mandatory resources have also been acquired.
     */
    void resourcesGranted(const QList<ResourcePolicy::ResourceType> &grantedOptionalResources);
    /**
     * This signal is emitted as a response to the acquire() request, in the
     * case where one or more of the mandatory resources were not available.
     */
    void resourcesDenied();
    /**
     * This signal is emitted as a response to the release() request.
     */
    void resourcesReleased();
    /**
      * This signal is emited when the manager releases our acquired resources,
      * so that we have to acquire them again when the user wishes to interact
      * with us.
      */
    void resourcesReleasedByManager();
    /**
     * This signal is emitted when some other program with a higher priority
     * supersedes us, and as a result we loose (some of) our resources.
     * It is very important to connect to this signal as it is signaling when
     * the acquired resources shouldn't be used anymore.
     */
    void lostResources();

    /**
     * Subscribe to this signal to receive error notifications,
     * particularly security errors.
     */
    void errorCallback(quint32, const char*);

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
    bool pendingAudioProperties;
    bool haveAudioProperties;

    void registerAudioProperties();

private slots:
    void connectedHandler();
    void handleGranted(quint32);
    void handleDeny();
    void handleReleased();
    void handleResourcesLost(quint32);
    void handleResourcesBecameAvailable(quint32);

    void handleAudioPropertiesChanged(const QString &group, quint32 pid,
                                      const QString &name, const QString &value);

};
}

#endif

