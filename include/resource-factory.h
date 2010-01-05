#ifndef RESOURCEFACTORY_H
#define RESOURCEFACTORY_H

#include <QObject>
#include <QDBusConnection>
#include "resource-types.h"

class Resource;

class ResourceFactory: public QObject
{
   Q_OBJECT
   Q_DISABLE_COPY( ResourceFactory );
private:
   QDBusConnection sessionBusConnection;
public:
   ResourceFactory(QObject *parent = 0);
   ~ResourceFactory();
   Resource * createResource(enum ResourceTypes::ResourceClass applicationClass,
			     quint16 requestedResources=ResourceTypes::AudioResource);
};

#endif
