#ifndef RESOURCE_H
#define RESOURCE_H

#include <QObject>
#include <QPointer>
#include "resource-library.h"
#include "resource-types.h"

using namespace ResourceTypes;

/**
 * This class represents a (set of) requested resource(s). It is created through
 * the ResourceFactory which knows how to connect it to the underying ResourceLibrary.
 * This class is responsible for managing the requested resource and is the way
 * for the application to communicate with the Resource Policy.
 *
 * If a program wants to use different sets of resources, e.g. a media player
 * wanting to play either music or video (audio + video resources), it would then
 * create multiple resource objects from the ResourceFactory.
 * 
 * Here is an example of how a media player might use this class. First off you
 * are going to need a \ref ResourceFactory which you use to create a \ref Resource.
 * The Resource has a number of signals, most are not strictly mandatory to connect
 * to, but in order to have it work correctly you should at least connect to the
 * connectedToServer() and stateChanged() signals, see this example.
 * \code
 *   resourceFactory = new ResourceFactory(this);
 *   QVERIFY(resourceFactory != NULL);
 *   Resource *resource = resourceFactory->createResource(MediaClass, AudioResource|VideoResource);
 *   if(resource == NULL) doError();
 *   QObject::connect(resource, SIGNAL(connectedToServer()),
 *                    &myResourceHandler, SLOT(connectedToServer()));
 *   QObject::connect(resource, SIGNAL(stateChanged(enum ResourceState)),
 *                    &myResourceHandler, SLOT(stateChanged(enum ResourceState)));
 *   resource->connectToServer();
 * \endcode
 * Then the resource handler would have methods like these to handle the signals:
 * \code
 * void MediaPlayerResourceHandler::connectedToServer()
 * {
 *     //We are now connected to the Policy Server, we can start requesting.
 *     this->reserve();
 * }
 * 
 * void MediaPlayerResourceHandler::stateChanged(enum ResourceState)
 * {
 *     if(resourceState == ResourceTypes::NotOwnedState) {
 *         this->mediaStream->stopMedia();
 *     }
 *     else if(resourceState == ResourceTypes::NotOwnedState) {
 *         //We now own the audio and video devices, so we can
 *         // freely start playing.
 *         this->mediaStream->startMedia();
 *     }
 * }
 * \endcode
 */
class Resource: public QObject
{
   Q_OBJECT
   Q_DISABLE_COPY( Resource );
   friend class ResourceFactory;
   friend class TestResource;
private:
   //! \internal The Class that the application belongs to.
   enum ResourceClass resourceClass;
   //! \internal A bit mask of ResourceTypes::ResourceType types which determine the
   //! the resources which we are interested in.
   quint16 resourceType;
   //! \internal The ResourceLibrary low-level protocol library wrapper.
   ResourceLibrary *resourceLibrary;
   //! The current state of the resource, whether we have exclusive access or not.
   enum ResourceState resourceState;

   //! \internal Private Constructor to prevent accidental mis-use
   Resource(enum ResourceClass requestedClass, quint16 requestedResources, QObject *parent=0);
   //! \internal Initializes the resource library and the low-level protocol library.
   bool initialize(ResourceLibrary *library);
public:
   virtual ~Resource();

   //! Returns true if we have exclusive access to our resources (as a whole)
   bool isReserved() const;
   //! Method used to query whether we have a given resource type in our set.
   bool hasResource(enum ResourceType resourceType) const;

   //! The application class that was registered to this resource.
   enum ResourceClass applicationClass() const;
   //! Returns the \ref ResourceTypes::ResourceType bit mask of resources.
   quint16 resources() const;

   //! Connects and registeres with the Resource Policy Server. The connectedToServer()
   //! signal is emited when the connection is successful.
   bool connectToServer();

   //! Disconnects from the Resource Policy server.
   void disconnectFromServer();

   //! \internal Stores the new state and emits a stateChanged() signal.
   void handleStateChange(enum ResourceState newState);
   //! \internal Emits the reservable() signal.
   void emitReservable();

   //! Reserve exclusive access to our resources.
   //! \return true when the message was successfully sent to the server.
   virtual bool reserve();
   //! Ask server to relinquish exclusive access to our resources.
   //! \return true when the message was successfully sent to the server.
   virtual bool release();
   //! Query the server of our current state. The stateChanged() signal will
   //! be emited when the server responds.
   virtual bool requestState();

   //! Set the global Mute state.
   virtual bool setMute();
   //! unset the global mute state.
   virtual bool unsetMute();
   //! Query the server for the global mute state. The reply will come
   //! via the muteState() signal.
   virtual bool requestMute();

   //! Set the global privacy override, i.e. pipe all audio to the public
   //! audio device (IHF speakers).
   virtual bool setPrivacyOverride();
   //! Unset the global privacy override, i.e. pipe audio to the private
   //! headset or eaprpiece if policy dictates so.
   virtual bool unsetPrivacyOverride();
   //! Query the server for the global provacy override state. The reply
   //! comes via the privacyOverride() signal.
   virtual bool requestPrivacyOverride();
   
   //! Set the global bluetooth override, i.e. Do not route audio to the
   //! bluetooth device even if it is connected.
   virtual bool setBluetoothOverride();
   //! Unset the global bluetooth override, i.e. audio is routed to a
   //! connected bluetooth headset if policy dictates so.
   virtual bool unsetBluetoothOverride();
   //! Query the server for the global bluetooth override flag. The reply
   //! comes via the bluetoothOverrideState() signal.
   virtual bool requestBluetoothOverride();

   //! Let the Policy Server know the process ID (PID) of the process which
   //! will do the actual media playing, in case it differs from the PID
   //! of the owner of this object.
   virtual bool setPid(quint32 pid);
   //! Tell policy the stream name of the media stream that this program uses,
   //! for the same reasons as above.
   virtual bool setStreamName(const QString & name);

signals:
   //! Emited when we are connected to the server.
   void connectedToServer();
   //! Emited when the server notifies us that another process has precedence
   //! over us. When receiving this signal with the ResourceTypes::NotOwnedState
   //! the program should immediately stop using the resources it had reserved
   //! (if it is in use). When receiving this signal with the
   //! ResourceTypes::OwnedState, it means that we have received access to the
   //! resources we asked for and are free to start to use them.
   void stateChanged(enum ResourceState newState);
   //! This is a hint by the Policy Server that our resources are free to use.
   //! It means we can expect a reserve() to result in a
   //! stateChanged(ResourceTypes::OwnedState) signal.
   void reservable();
   //! This signal shows the global mute state.
   void muteStateChanged(bool oldMuteState, bool newMuteState);
   //! This signal shows the global privacy override state.
   void privacyOverrideChanged(bool PrivacyState, bool newPrivacyState);
   //! This signal shows the global bluetooth override state.
   void bluetoothOverrideChanged(bool oldBluetoothState, bool newBluetoothState);
};

#endif
    
