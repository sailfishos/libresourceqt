#ifndef MOCK_RESOURCE_LIBRARY_H
#define MOCK_RESOURCE_LIBRARY_H

#include <QObject>
#include "resource-library.h"

class MockResourceLibrary: public QObject, public ResourceLibrary
{
   Q_OBJECT
private:
   bool initializeFails;
public:
   MockResourceLibrary(QObject *parent=0,bool makeInitFail=false);
   virtual ~MockResourceLibrary();
   void makeInitializeFail();
   bool initialize();
};

#endif
