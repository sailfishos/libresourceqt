#include "mock-resource-library.h"

MockResourceLibrary::MockResourceLibrary(QObject *parent,
					 bool makeInitializationFail,
					 bool makeConnectingToServerFail)
    : QObject(parent), initializeFails(makeInitializationFail),
      connectToServerFails(makeConnectingToServerFail)
{
}

MockResourceLibrary::~MockResourceLibrary()
{
}

void MockResourceLibrary::makeInitializeFail()
{
    initializeFails = true;
}

bool MockResourceLibrary::initialize()
{
    if(initializeFails==false)
	return true;
    else
	return false;
}

void MockResourceLibrary::makeServerConnectFail()
{
    connectToServerFails = true;
}

bool MockResourceLibrary::connectToServer()
{
    if(connectToServerFails == false)
	return true;
    else
	return false;
}
