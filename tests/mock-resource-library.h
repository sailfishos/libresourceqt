#ifndef MOCK_RESOURCE_LIBRARY_H
#define MOCK_RESOURCE_LIBRARY_H

#include <QObject>
#include "resource-library.h"
#include "resource.h"

class MockResourceLibrary: public QObject, public ResourceLibrary
{
   Q_OBJECT
private:
   bool initializeFails;
   bool connectToServerFails;
   Resource *resource;
   bool reserveFails;
public:
   MockResourceLibrary(Resource *resource=0,bool makeInitializationFail=false,
		       bool makeConnectingToServerFail=false, bool makeReserveFail=false);
   virtual ~MockResourceLibrary();
   void makeInitializeFail();
   void makeServerConnectFail();
   bool initialize();
   bool connectToServer();

   bool reserve();
   void makeReserveFail();
};

#endif
