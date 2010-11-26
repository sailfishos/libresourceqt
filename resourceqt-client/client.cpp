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

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QTextStream>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>

#include "client.h"

#define outputln output << "\n"

using namespace ResourcePolicy;

QMap<QString, CommandListArgs> Client::commandList;

CommandListArgs::CommandListArgs()
        : args(), help()
{
}


CommandListArgs::CommandListArgs(const QString &arguments, const QString &helpText)
        : args(arguments), help(helpText)
{
}

CommandListArgs::~CommandListArgs()
{
}

Client::Client()
        : QObject(), standardInput(stdin, QIODevice::ReadOnly), stdInNotifier(0, QSocketNotifier::Read), pendingAddAudio(false), applicationClass(),
        resourceSet(NULL), output(stdout)
{
    commandList["help"] = CommandListArgs("", "print this help message");
    commandList["quit"] = CommandListArgs("", "exit application");
    commandList["free"] = CommandListArgs("", "destroy and free the resources");
    commandList["acquire"] = CommandListArgs("", "acquire required resources");
    commandList["release"] = CommandListArgs("", "release resources");
    commandList["update"] = CommandListArgs("update all[:opt] where 'all' and 'opt' are comma separated resources",
                                               "update the resource set by specifying the new set");
    commandList["audio"] = CommandListArgs("pid <pid> | group <audio group> | tag <name> <value>", "set audio properties");
    commandList["addaudio"] = CommandListArgs("<audio group> <pid> <tag name> <tag value>", "Add an audio resource and set the properties");
    commandList["show"] = CommandListArgs("", "show resources");

}

Client::~Client()
{
    delete resourceSet;
}

void Client::showPrompt()
{
    output << "resource-Qt> " << flush;
}

bool Client::initialize(const CommandLineParser &parser)
{
    QSet<ResourcePolicy::ResourceType> allResources;
    QSet<ResourcePolicy::ResourceType> optionalResources;

    if (parser.shouldAlwaysReply()) {
        output << "client: AlwaysReply" << endl;
    }

    if (parser.shouldAutoRelease()) {
        output << "client: AutoRelease" << endl;
    }

    resourceSet = new ResourceSet(parser.resourceApplicationClass(), this,
                                  parser.shouldAlwaysReply(),
                                  parser.shouldAutoRelease());
    if (resourceSet == NULL) {
        return false;
    }

    allResources.unite(parser.resources());
    optionalResources.unite(parser.optionalResources());
    foreach(ResourcePolicy::ResourceType resource, allResources) {
        resourceSet->addResource(resource);
        if (optionalResources.contains(resource)) {
            resourceSet->resource(resource)->setOptional();
        }
    }

    if (!connect(resourceSet, SIGNAL(resourcesGranted(QList<ResourcePolicy::ResourceType>)),
                 this, SLOT(resourceAcquiredHandler(QList<ResourcePolicy::ResourceType>)))) {
        return false;
    }

    if (!connect(resourceSet, SIGNAL(resourcesDenied()), this, SLOT(resourceDeniedHandler()))) {
        return false;
    }

    if (!connect(resourceSet, SIGNAL(lostResources()), this, SLOT(resourceLostHandler()))) {
        return false;
    }
    if (!connect(resourceSet, SIGNAL(resourcesReleased()), this, SLOT(resourceReleasedHandler()))) {
        return false;
    }
    if (!connect(resourceSet, SIGNAL(resourcesBecameAvailable(const QList<ResourcePolicy::ResourceType> &)),
                 this, SLOT(resourcesBecameAvailableHandler(const QList<ResourcePolicy::ResourceType> &)))) {
        return false;
    }
    if (!connect(&stdInNotifier, SIGNAL(activated(int)), this, SLOT(readLine(int)))) {
        return false;
    }
    if (!connect(resourceSet , SIGNAL(connectedToManager()), this, SLOT(stopConnectTimerHandler()))) {
        return false;
    }
    if (!connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()),
                 this, SLOT(doExit()))) {
        return false;
    }

    start_timer();
    resourceSet->initAndConnect();
    output << "connecting...accepting input" << endl;
    showPrompt();
    return true;
}

void Client::doExit()
{
    if (resourceSet != NULL)
        resourceSet->release();
}


void Client::modifyResources(QString resString)
{
    //resString example: [mand_resources:opt_resources] res1,res2,res3:res1,res3
   if ( resString.isEmpty() || resString.isNull())
   {
       qDebug("Client::modifyResources(): no resources in string.");
       return;
   }
   QStringList newAllAndOpt = resString.split(":");

   if (newAllAndOpt.size()==1)
       qDebug("There are only manditory resources.");

   //Every optional res. is also in allSet
   QSet<ResourcePolicy::ResourceType> newAllSet;
   QSet<ResourcePolicy::ResourceType> newOptSet;

   if ( !CommandLineParser::parseResourceList(newAllAndOpt.at(0), newAllSet) )
       return;

   if (newAllAndOpt.size()>1)
   {
       if ( !CommandLineParser::parseResourceList(newAllAndOpt.at(1), newOptSet) )
           return;
       //If the user forgot to add resource to all when specifying optional -> add to all.
       foreach(ResourceType newOptRes, newOptSet)
       {
           if ( !newAllSet.contains(newOptRes) )
           {
               qDebug("Client::modifyResources(): optional resources should be added to all as well.");
               newAllSet.insert(newOptRes);
           }
       }
   }

   QSet<ResourceType>::iterator newResIt = newAllSet.begin();
   QList<Resource*>             resList  = resourceSet->resources();

   //Check if new resources are in current resource set.
   while (newResIt != newAllSet.end())
   {
        ResourceType newRes = *newResIt;

        if ( resourceSet->contains(newRes) )
        {
            if ( resourceSet->resource(newRes)->isOptional() && !newOptSet.contains(newRes) ) {
                //New manditory is in set, but is not optional in the new set.
                resourceSet->resource(newRes)->setOptional(false);
            }
            else if ( !resourceSet->resource(newRes)->isOptional() && newOptSet.contains(newRes) )
            {
                //New manditory is in set, but is optional.
                resourceSet->resource(newRes)->setOptional(true);
            }
        }
        else
        {   //Add new resource.
            resourceSet->addResource(newRes);

            if ( newOptSet.contains(newRes) )
                resourceSet->resource(newRes)->setOptional(true);

        }
        ++newResIt;
    }

    //Check if there are current resources not in the new set (i.e. removed).
    foreach(Resource* resource, resList)
        if ( !newAllSet.contains(resource->type()) )
            resourceSet->deleteResource(resource->type());

}


const char * resourceTypeToString(ResourceType type)
{
    switch (type) {
    case AudioPlaybackType:
        return "AudioPlayback";
    case AudioRecorderType:
        return "AudioRecording";
    case VideoPlaybackType:
        return "VideoPlayback";
    case VideoRecorderType:
        return "VideoRecording";
    case VibraType:
        return "Vibra";
    case LedsType:
        return "Leds";
    case BacklightType:
        return "Backlight";
    case SystemButtonType:
        return "SystemButton";
    case LockButtonType:
        return "LockButton";
    case ScaleButtonType:
        return "ScaleButton";
    case SnapButtonType:
        return "SnapButton";
    case LensCoverType:
        return "LensCover";
    case HeadsetButtonsType:
        return "HeadsetButtons";
    default:
        return "Unknown/Invalid Resource";
    }
}

void Client::showResources(const QList<ResourceType> &resList)
{
    outputln << "Resource Set:\n";
    foreach(ResourceType resource, resList) {
        output << "\t" << resourceTypeToString(resource) << endl;
    }
}

void Client::showResources(const QList<Resource*> &resList)
{
    outputln << "Resource Set:\n";
    foreach(Resource* resource, resList) {
        output << "\t" << resourceTypeToString(resource->type());
        if (resource->isOptional())
            output << " (optional)";
        if (resource->isGranted())
            output << " (granted)";
        output << endl;
    }
}

void Client::stopConnectTimerHandler()
{
    long int ms = stop_timer();
    if (ms > 0) {
        outputln << "Register took " << ms << "ms" << endl;
    }

}

void Client::resourceAcquiredHandler(const QList<ResourceType>&)
{
    long int ms = stop_timer();
    if (ms > 0) {
        outputln << "Operation took " << ms << "ms" << endl;
    }

    QList<Resource*> list = resourceSet->resources();
    if (!list.count()) {
        qFatal("Resource set is empty, but we received a grant. Possible bug?");
    }
    else {
        QList<ResourceType> grantedResources;
        foreach(ResourcePolicy::Resource *resource, list) {
            if (resource->isGranted()) {
                grantedResources << resource->type();
            }
        }
        output << "granted:" << grantedResources << endl;
    }
    showPrompt();
}

void Client::resourceDeniedHandler()
{
    long int ms = stop_timer();
    if (ms > 0) {
        outputln << "Operation took " << ms << "ms" << endl;
    }
    QList<Resource*> allResources = resourceSet->resources();
    output << "denied:" << allResources << endl;
    showPrompt();
}

void Client::resourceLostHandler()
{
    long int ms = stop_timer();
    if (ms > 0) {
        outputln << "Operation took " << ms << "ms" << endl;
    }

    QList<Resource*> allResources = resourceSet->resources();
    outputln << "lost:" << allResources << endl;
    showPrompt();
}

void Client::resourceReleasedHandler()
{
    long int ms = stop_timer();
    if (ms > 0) {
        outputln << "Operation took " << ms << "ms" << endl;
    }

    QList<Resource*> allResources = resourceSet->resources();
    outputln << "released:"<< allResources << endl;
    showPrompt();
}

void Client::resourcesBecameAvailableHandler(const QList<ResourcePolicy::ResourceType> &availableResources)
{
    if (pendingAddAudio) {
        pendingAddAudio = false;
        long int ms = stop_timer();
        if (ms > 0) {
            outputln << "Operation took " << ms << "ms" << endl;
        }
    }
    outputln << "advice:" << availableResources << endl;
    showPrompt();
}

void Client::readLine(int)
{
    QString line = standardInput.readLine();
    if (line.isNull() || line.isEmpty()) {
        showPrompt();
        return;
    }
    QTextStream input(&line, QIODevice::ReadOnly);
    QString command;
    input >> command;
    if (command.isNull() || command.isEmpty()) {
        qDebug("Unable to read a command");
        return;
    }

    if ((command == "quit") || (command == "exit")) {
        QCoreApplication::quit();
        return;
    }
    else if (command == "help") {
        output << "Available commands:\n";
        QMap<QString, CommandListArgs>::const_iterator i =
            commandList.constBegin();
        while (i != commandList.constEnd()) {
            output << qSetFieldWidth(10) << right << i.key()
            << qSetFieldWidth(1) << " "
            << qSetFieldWidth(55) << left << i.value().args
            << qSetFieldWidth(0) << i.value().help << endl;
            ++i;
        }
    }
    else if (command == "show") {
        if (!resourceSet) {
            qCritical("%s failed!", qPrintable(command));
        }
        else {
            QList<Resource*> list = resourceSet->resources();
            if (!list.count()) {
                output << "Resource set is empty, use add command to add some."
                << endl;
            }
            else {
                showResources(list);
            }
        }
    }
    else if (command == "acquire") {
        start_timer();
        if (!resourceSet || !resourceSet->acquire()) {
            stop_timer();
            qCritical("%s failed!", qPrintable(command));
        }
    }
    else if (command == "release") {
        start_timer();
        if (!resourceSet || !resourceSet->release()) {
            stop_timer();
            qCritical("%s failed!", qPrintable(command));
        }
    }

    else if (command == "update") {

        QString resourceList;
        input >> resourceList;

        if (!resourceSet)
            qCritical("%s failed!", qPrintable(command));

        if (resourceList.isEmpty() || resourceList.isNull())
        {
             qCritical("%s failed! List of desired resources is missing. Use help.",
                       qPrintable(command));
        }
        else
        {
            start_timer();
            modifyResources(resourceList);

            if (!resourceSet->update())
                qCritical("%s failed!", qPrintable(command));
        }

    }
    else if (command == "audio") {
        QString what, group, tagName, tagValue;
        quint32 pid = 0;
        input >> what;

        if (what.isEmpty() || what.isNull()) {
            output << "Not enough parameters! See help" << endl;
        }
        else {
            Resource *resource = resourceSet->resource(AudioPlaybackType);
            AudioResource *audioResource = static_cast<AudioResource*>(resource);
            qDebug("resource = %p audioResource = %p", resource, audioResource);
            if (audioResource == NULL) {
                output << "No AudioResource available in set!" << endl;
            }
            else {
                if (what == "group") {
                    input >> group;
                    audioResource->setAudioGroup(group);
                }
                else if (what == "pid") {
                    input >> pid;
                    if (pid != 0) {
                        qDebug("Setting audio PID to %u", pid);
                        audioResource->setProcessID(pid);
                    }
                    else {
                        output << "Bad pid parameter!" << endl;
                    }
                }
                else if (what == "tag") {
                    input >> tagName >> tagValue;
                    if (tagName.isEmpty() || tagName.isNull() ||
                            tagValue.isEmpty() || tagValue.isNull()) {
                        output << "tag requires 2 parameters name and value. See help"
                        << endl;
                    }
                    else {
                        audioResource->setStreamTag(tagValue, tagName);
                    }
                }
                else {
                    output << "Unknown audio command!";
                }
            }
        }
    }
    else if (command == "addaudio") {
        QString group, tagName, tagValue;
        quint32 pid = 0;
        input >> group >> pid >> tagName >> tagValue;

        if (group.isEmpty() || (pid == 0) || tagName.isEmpty() || tagValue.isEmpty()) {
            output << "Invalid parameters! See help!" << endl;
        }
        else {
            AudioResource *audioResource = new AudioResource(group);
            if (audioResource == NULL) {
                output << "Failed to create an AudioResource object!" << endl;
            }
            else {
                audioResource->setProcessID(pid);
                audioResource->setStreamTag(tagName, tagValue);
                pendingAddAudio = true;
                start_timer();
                resourceSet->addResourceObject(audioResource);
            }
        }
    }
    else if (command == "free") {
        delete resourceSet;
        resourceSet = new ResourceSet(applicationClass);
    }
    else {
        output << "unknown command '" << command << "'" << endl;
    }

    showPrompt();
}

QTextStream & operator<<(QTextStream &output,
                         const QList<ResourcePolicy::Resource *>resources)
{
    char separator = ' ';
    foreach(Resource* resource, resources) {
        output << separator << resourceTypeToString(resource->type());
        separator = ',';
    }
    return output;
}

QTextStream & operator<< (QTextStream &output,
                          const QList<ResourcePolicy::ResourceType>resources)
{
    char separator = ' ';
    foreach(ResourceType resource, resources) {
        output << separator << resourceTypeToString(resource);
        separator = ',';
    }
    return output;
}

