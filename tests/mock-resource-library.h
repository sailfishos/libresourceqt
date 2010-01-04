#ifndef MOCK_RESOURCE_LIBRARY_H
#define MOCK_RESOURCE_LIBRARY_H

#include <QObject>
#include "resource-library.h"

class MockResourceLibrary: public QObject, public ResourceLibrary
{
   Q_OBJECT
private:
   bool initializeFails;
   bool connectToServerFails;
public:
   MockResourceLibrary(QObject *parent=0,bool makeInitializationFail=false,
		       bool makeConnectingToServerFail=false);
   virtual ~MockResourceLibrary();
   void makeInitializeFail();
   void makeServerConnectFail();
   bool initialize();
   bool connectToServer();
};

#endif
