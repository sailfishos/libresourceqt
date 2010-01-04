#ifndef RESOURCE_H
#define RESOURCE_H

#include <QObject>
#include <QPointer>
#include "resource-library.h"
#include "resource-types.h"

class Resource: public QObject
{
   Q_OBJECT
   Q_DISABLE_COPY( Resource );
   friend class ResourceFactory;
   friend class TestResource;
private:
   enum ResourceClass resourceClass;
   quint16 flags;
   ResourceLibrary *resourceLibrary;

   bool initialize(ResourceLibrary *library);
   Resource(enum ResourceClass type, quint16 resourceFlags, QObject *parent=0);
public:
   virtual ~Resource();

   enum ResourceClass getResourceClass();
   quint16 getResourceFlags();

   bool connectToServer();

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
};

#endif
    
