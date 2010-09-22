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

#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H
#include <QString>
#include <QList>
#include <QSet>
#include <QHash>
#include <QTextStream>
#include <policy/resource.h>

class CommandLineParser
{
public:
    CommandLineParser();
    ~CommandLineParser();

public:
    bool parseArguments();

    static bool parseResourceList(const QString &resourceListStr,
                                  QSet<ResourcePolicy::ResourceType> &resourceList);

    const QSet<ResourcePolicy::ResourceType>& resources() const;
    const QSet<ResourcePolicy::ResourceType>& optionalResources() const;
    QString resourceApplicationClass() const;
    bool shouldAutoRelease() const;
    bool shouldAlwaysReply() const;
    bool shouldBeVerbose() const;

private:
    QSet<ResourcePolicy::ResourceType> allResources;
    QSet<ResourcePolicy::ResourceType> optResources;
    static QHash<QString, ResourcePolicy::ResourceType> resourceValues;
    QString applicationClass;
    bool autoRelease;
    bool alwaysReply;
    bool verbose;
    bool allowUnkownResourceClass;
    QTextStream output;

    bool parseClassString(const QString &str);
    bool parseModeValues(const QString &modeListStr);

    void usage();

};
#endif

