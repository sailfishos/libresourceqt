#ifndef RESOURCEFACTORY_H
#define RESOURCEFACTORY_H

#include <QObject>
#include <QDBusConnection>
#include "resource-types.h"

class ResourceLibrary;
class Resource;

class ResourceFactory: public QObject
{
   Q_OBJECT
private:
   QDBusConnection sessionBusConnection;
public:
   ResourceFactory(QObject *parent = 0);
   ~ResourceFactory();
   Resource * createResource(enum ResourceClass type, quint16 usageFlags=RP_FLAGS_AUDIO);
};

#endif
