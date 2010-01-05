#ifndef RESOURCE_H
#define RESOURCE_H

#include <QObject>
#include <QPointer>
#include "resource-library.h"
#include "resource-types.h"

using namespace ResourceTypes;

class Resource: public QObject
{
   Q_OBJECT
   Q_DISABLE_COPY( Resource );
   friend class ResourceFactory;
   friend class TestResource;
private:
   enum ResourceClass resourceClass;
   quint16 resourceType;
   ResourceLibrary *resourceLibrary;
   enum ResourceState resourceState;

   Resource(enum ResourceClass requestedClass, quint16 requestedResources, QObject *parent=0);
   bool initialize(ResourceLibrary *library);
public:
   virtual ~Resource();

   bool hasExclusiveAccess() const;
   bool hasResource(enum ResourceType resourceType) const;

   enum ResourceClass applicationClass() const;
   quint16 resources() const;

   bool connectToServer();

   void handleStateChange(enum ResourceState newState);

   virtual bool requestExclusiveAccess();
   virtual bool releaseExclusiveAccess();
   virtual bool getExclusiveAccessState();
/*
   virtual bool setMute();
   virtual bool unsetMute();
   virtual bool requestMute();
   
   virtual bool setPrivacyOverride();
   virtual bool unsetPrivacyOverride();
   virtual bool requestPrivacyOverride();
   
   virtual bool setBluetoothOverride();
   virtual bool unsetBluetoothOverride();
   virtual bool requestBluetoothOverride();

   virtual bool setPID(pid_t pid);
   virtual bool setStreamName(const QString & name);
*/
   // completed and discarded from libplayback? what are those?
signals:
   void connectedToServer();
   void stateChanged(enum ResourceState newState);
};

#endif
    
