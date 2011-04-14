/*************************************************************************
This file is part of libresourceqt

Copyright (C) 2010 Nokia Corporation.

This library is free software; you can redistribute
it and/or modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation
version 2.1 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
USA.
*************************************************************************/

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
#ifdef TEST_RESOURCE_ENGINE_H
    friend class ::TestResourceEngine;
#endif

public:
    ResourceEngine(ResourceSet *resourceSet);
    ~ResourceEngine();

    bool initialize();

    bool connectToManager();
    bool disconnectFromManager();
    bool isConnectedToManager();
    bool isConnectingToManager();

    bool acquireResources();
    bool releaseResources();
    bool updateResources();

    bool registerAudioProperties(const QString &audioGroup, quint32 pid,
                                  const QString &name, const QString &value);

    bool registerVideoProperties(quint32 pid);


    void handleConnectionIsUp(resconn_t *connection);

    void disconnected();
    void receivedGrant(resmsg_notify_t *notifyMessage);
    void receivedAdvice(resmsg_notify_t *notifyMessage);
    void receivedRelease(resmsg_notify_t *notifyMessage);

    void handleStatusMessage(quint32 requestNo);
    void handleError(quint32 requestNo, qint32 code, const char *message);

    quint32 id();
    bool toBeDeleted();

signals:
    void resourcesBecameAvailable(quint32 bitmaskOfAvailableResources);
    void resourcesGranted(quint32 bitmaskOfGrantedResources);
    void resourcesDenied();
    void resourcesReleased();
    void resourcesLost(quint32 bitmaskOfGrantedResources);
    void connectedToManager();
    void disconnectedFromManager();
    void errorCallback(quint32 code, const char* );
    void resourcesReleasedByManager();
    void updateOK();

private:
    bool connected;
    ResourceSet *resourceSet;
    DBusConnection *dbusConnection;
    resset_t *libresourceSet;
    quint32 requestId;
    QMap<quint32, resmsg_type_t> messageMap;
    QMap<quint32, bool> wasInAcquireMode;
    quint32 connectionMode;
    static quint32 libresourceUsers;
    static resconn_t *libresourceConnection;
    quint32 identifier;
    bool aboutToBeDeleted;
    bool isConnecting;
};
}

#endif

