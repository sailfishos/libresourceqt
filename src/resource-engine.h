#ifndef RESOURCE_ENGINE_H
#define RESOURCE_ENGINE_H

#include <QObject>
#include <QMap>
#include <dbus/dbus.h>
#include <res-conn.h>
#include "resource-set.h"

namespace ResourcePolicy {

class ResourceEngineData;

class ResourceEngine: public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( ResourceEngine )
public:
    ResourceEngine(ResourceSet *resourceSet=NULL);
    ~ResourceEngine();

    bool initialize();

    bool connect();
    bool disconnect();
    bool isConnected();

    bool acquireResources();
    bool releaseResources();
    bool updateResources();

    bool registerAudioProperties(quint32 pid, QString streamName);

    void handleConnectionIsUp();

signals:
    void resourcesBecameAvailable(QList<Resource> availableResources);
    void resourcesAcquired(QList<Resource> grantedResources);
    void resourcesDenied();
    void lostResources();
    void connectedToManager();
    void disconnectedFromManager();

private:
    bool connected;
    ResourceSet *resourceSet;
    DBusConnection *dbusConnection;
    resconn_t *libresourceConnection;
    resset_t *libresourceSet;
    quint32 requestId;
    QMap<quint32, resmsg_type_t> messageMap;
};
}

#endif
