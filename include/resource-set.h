#ifndef RESOURCE_SET_H
#define RESOURCE_SET_H

#include "resource.h"
#include <QString>
#include <QObject>
#include <QSet>

namespace ResourcePolicy
{
   class ResourceSet: public QObject
   {
      Q_OBJECT
      Q_DISABLE_COPY( ResourceSet )
  public:
      ResourceSet(const QString &applicationClass);
      ~ResourceSet();

      void addResource(const Resource &resource);
      void addResources(const QSet<Resource> &resources);
      void setResources(const QSet<Resource> &resources);
      QSet<Resource> resources();

      bool contains(const Resource &resource) const;
      bool contains(const QSet<Resource> &resources) const;

      quint32 id() const;
  signals:
      void resourcesBecameAvailable(QSet<Resource> resources);
      void resourcesAcquired(QSet<Resource> grantedResources);
      void resourcesDenied();
      void lostResources();

  private:
      quint32 identifier;
      const QString applicationClass;
      QSet<Resource> resourceSet;
   };
}

#endif
