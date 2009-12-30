#include "mock-resource-library.h"

MockResourceLibrary::MockResourceLibrary(QObject *parent, bool makeInitFail)
    : QObject(parent), initializeFails(makeInitFail)
{
}

MockResourceLibrary::~MockResourceLibrary()
{
}

void MockResourceLibrary::makeInitializeFail()
{
    initializeFails=true;
}

bool MockResourceLibrary::initialize()
{
    return !initializeFails;
}
