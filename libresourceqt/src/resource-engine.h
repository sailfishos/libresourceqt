#ifndef RESOURCE_ENGINE_H
#define RESOURCE_ENGINE_H

#include <QObject>
#include <QMap>
#include <QString>
#include <dbus/dbus.h>
#include <res-conn.h>
#include <policy/resource-set.h>
#include <dbusconnectioneventloop.h>

namespace ResourcePolicy {

quint32 resourceTypeToLibresourceType(ResourceType type);

class ResourceEngine: public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( ResourceEngine )
#ifndef QT_NO_DEBUG
    friend class TestResourceEngine;
#endif

public:
    ResourceEngine(ResourceSet *resourceSet);
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

    void disconnected();
    void receivedGrant(resmsg_notify_t *notifyMessage);
    void receivedAdvice(resmsg_notify_t *notifyMessage);

    void handleStatusMessage(quint32 requestNo);
    void handleError(quint32 requestNo, qint32 code, const char *message);

    void setMode(quint32 newMode);

signals:
    void resourcesBecameAvailable(QList<ResourceType> availableResources);
    void resourcesAcquired(quint32 bitmaskOfGrantedResources);
    void resourcesDenied();
    void connectedToManager();
    void disconnectedFromManager();

private:
    
    bool connected;
    ResourceSet *resourceSet;
    DBusConnection *dbusConnection;
    DBUSConnectionEventLoop *dbusEngine;
    resconn_t *libresourceConnection;
    resset_t *libresourceSet;
    quint32 requestId;
    QMap<quint32, resmsg_type_t> messageMap;
    quint32 mode;
};
}

#endif
