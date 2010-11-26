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

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <QObject>
#include <QtCore/QTextStream>
#include <QSocketNotifier>

#include <stdint.h>
#include <policy/resource-set.h>
#include <sys/resource.h>

#include "commandlineparser.h"
#include "time-stat.h"

class CommandListArgs
{
public:
    CommandListArgs(const QString &arguments, const QString &helpText);
    CommandListArgs();
    ~CommandListArgs();
    QString args;
    QString help;
};

class Client : public QObject
{
    Q_OBJECT

public:
    Client();
    ~Client();

    bool initialize(const CommandLineParser &commandParser);
    static uint32_t parseResourceList(QString resourceListStr);

private slots:
    void resourceAcquiredHandler(const QList<ResourcePolicy::ResourceType>& grantedResList);
    void resourceDeniedHandler();
    void resourceLostHandler();
    void resourceReleasedHandler();
    void resourcesBecameAvailableHandler(const QList<ResourcePolicy::ResourceType> &availableResources);
    void readLine(int);
    void doExit();
    void stopConnectTimerHandler();

private:
    QTextStream standardInput;
    QSocketNotifier stdInNotifier;
    bool pendingAddAudio;
    QString applicationClass;
    ResourcePolicy::ResourceSet *resourceSet;
    QTextStream output;

    static QMap<QString, CommandListArgs> commandList;

    ResourcePolicy::Resource* allocateResource(ResourcePolicy::ResourceType resource, bool optional);
    ResourcePolicy::ResourceType getResourceType(uint32_t resource);

    void showPrompt();
    void showResources(const QList<ResourcePolicy::ResourceType> &resList);
    void showResources(const QList<ResourcePolicy::Resource*> &resList);
    void modifyResources(QString resString);
};

QTextStream & operator<< (QTextStream &output,
                          const QList<ResourcePolicy::Resource*>resources);
QTextStream & operator<< (QTextStream &output,
                          const QList<ResourcePolicy::ResourceType>resources);
#endif

