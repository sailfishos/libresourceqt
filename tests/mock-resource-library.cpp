#include "mock-resource-library.h"

MockResourceLibrary::MockResourceLibrary(Resource *resource,
					 bool makeInitializationFail,
					 bool makeConnectingToServerFail,
					 bool makeReserveFail)
    : QObject(resource), initializeFails(makeInitializationFail),
      connectToServerFails(makeConnectingToServerFail),
      reserveFails(makeReserveFail)
{
    this->resource = resource;
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

bool MockResourceLibrary::reserve()
{
    if(!reserveFails) {
	resource->handleStateChange(OwnedState);
    }
    else {
	resource->handleStateChange(NotOwnedState);
    }
}

void MockResourceLibrary::makeReserveFail()
{
    reserveFails = true;
}
