#ifndef RESOURCEFACTORY_H
#define RESOURCEFACTORY_H

#include <QObject>
#include <QDBusConnection>
#include "resource-types.h"

class Resource;

/** This class is a helper class which should be used to create Resources. Note
 * that it will own all resources, and so should not be deleted before all
 * resources are ready to be released. The best way to acchieve this is by
 * makeing the ResourceHandler class own the factory.
 */
class ResourceFactory: public QObject
{
   Q_OBJECT
   Q_DISABLE_COPY( ResourceFactory );
private:
   //! \internal
   QDBusConnection sessionBusConnection;
public:
   ResourceFactory(QObject *parent = 0);
   ~ResourceFactory();
   /** This method creates a new \ref Resource object.
    * @param applicationClass The application class to tell the Resource Policy.
    * @param requestedResources A bit mask of the resources tha this application
    * wats as a set. If more than one resource is set in this bit mask, then
    * that set is treated as a whole, and cannot be separated. If the application
    * needs at times say only audio and at times audio and video, then several
    * \ref Resource objects need to be created.
    * @return Returns a pointer to a new Resource, or NULL if there is an error.
    */
   Resource * createResource(enum ResourceTypes::ResourceClass applicationClass,
			     quint16 requestedResources=ResourceTypes::AudioResource);
};

#endif
