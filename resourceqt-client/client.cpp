#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QTextStream>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>

#include "client.h"

using namespace ResourcePolicy;

QMap<QString, CommandListArgs> Client::commandList;

CommandListArgs::CommandListArgs()
    :args(), help()
{
}


CommandListArgs::CommandListArgs(const QString &arguments, const QString &helpText)
    :args(arguments), help(helpText)
{
}

CommandListArgs::~CommandListArgs()
{
}

Client::Client()
    : QObject(), standardInput(stdin, QFile::ReadOnly), applicationClass(),
      resourceSet(NULL), output(stdout)
{
    mainTimerID   = startTimer(0);
    commandList["help"] = CommandListArgs("", "print this help message");
    commandList["quit"] = CommandListArgs("", "exit application");
    commandList["free"] = CommandListArgs("", "destroy and free the resources");
    commandList["acquire"] = CommandListArgs("", "acquire required resources");
    commandList["release"] = CommandListArgs("", "release resources");
    commandList["update"] = CommandListArgs("", "update modified resource set after add or remove command");
    commandList["add"] = CommandListArgs("reslist [-o]","add reosurce list, if -o provided, set as optional");
    commandList["remove"] = CommandListArgs("reslist [-o]","remove reosurce list, if -o provided, removed only optional flag");
    commandList["audio"] = CommandListArgs("pid <pid> | group <audio group> | tag <name> <value>","set audio properties");
    commandList["addaudio"] = CommandListArgs("<audio group> <pid> <tag name> <tag value>","Add an audio resource and set the porpoerties");
    commandList["show"] = CommandListArgs("", "show resources");

}

Client::~Client()
{
    killTimer(mainTimerID);

    delete resourceSet;
}

void Client::showPrompt()
{
    output << "res-client> " << flush;
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
    foreach (ResourcePolicy::ResourceType resource, allResources) {
        resourceSet->addResource(resource);
        if (optionalResources.contains(resource)) {
            resourceSet->resource(resource)->setOptional();
        }
    }

    if (!connect(resourceSet, SIGNAL(resourcesGranted(QList<ResourcePolicy::ResourceType>)),
                 this, SLOT(resourceAcquiredHandler(QList<ResourcePolicy::ResourceType>))))
    {
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
                 this, SLOT(resourcesBecameAvailableHandler(const QList<ResourcePolicy::ResourceType> &))))
    {
        return false;
    }

    showPrompt();

    return true;
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
            return "VideoRegording";
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
    output << "Resource Set:\n";
    foreach (ResourceType resource, resList) {
        output << "\t" << resourceTypeToString(resource) << endl;
    }
}

void Client::showResources(const QList<Resource*> &resList)
{
    output << "Resource Set:\n";
    foreach (Resource* resource, resList) {
        output << "\t" << resourceTypeToString(resource->type());
        if (resource->isOptional())
            output << " (optional)";
        if (resource->isGranted())
            output << " (granted)";
        output << endl;
    }
}

void Client::resourceAcquiredHandler(const QList<ResourceType>& /*grantedResList*/)
{
    double ms = stop_timer();
    if( ms > 0.0 ) {
        output << "Operation took " << qSetRealNumberPrecision(2) << ms << "ms";
    }

    QList<Resource*> list = resourceSet->resources();
    if (!list.count()) {
        qFatal("Granted resource set is empty. Possible bug?");
    }
    else {
        output << "\nReceived a grant.\n";
        showResources(list);
    }
    showPrompt();
}

void Client::resourceDeniedHandler()
{
    double ms = stop_timer();
    if( ms > 0.0 ) {
        output << "Operation took " << qSetRealNumberPrecision(2) << ms << "ms";
    }

    output << "\nManager denies access to resources!" << endl;
    showPrompt();
}

void Client::resourceLostHandler()
{
    double ms = stop_timer();
    if( ms > 0.0 ) {
        output << "Operation took " << qSetRealNumberPrecision(2) << ms << "ms";
    }

    output << "\nLost resources from manager!" << endl;
    showPrompt();
}

void Client::resourceReleasedHandler()
{
    double ms = stop_timer();
    if( ms > 0.0 ) {
        output << "Operation took " << qSetRealNumberPrecision(2) << ms << "ms";
    }

    output << "\nAll resources released" << endl;
    showPrompt();
}

void Client::resourcesBecameAvailableHandler(const QList<ResourcePolicy::ResourceType> &availableResources)
{
    output << "Manager advice: These resources are available:\n";
    showResources(availableResources);
    showPrompt();
}

void Client::timerEvent(QTimerEvent*)
{
    bool quitFlag = false;

    fd_set stdinfd;
    FD_ZERO(&stdinfd);
    FD_SET(0, &stdinfd);
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    int ready = select(1, &stdinfd, NULL, NULL, &tv);
    if (ready > 0) {
        QString line = standardInput.readLine();
        if (!line.isNull()) {
            QStringList params = line.split(" ");
            if (params[0] == "quit") {
                quitFlag = true;
                QMetaObject::invokeMethod(QCoreApplication::instance(), "quit");
            }
            else if (params[0] == "help") {
                output << "Available commands:\n";
                QMap<QString, CommandListArgs>::const_iterator i = 
                    commandList.constBegin();
                while (i != commandList.constEnd()) {
                    output << qSetFieldWidth (10) << right << i.key()
                           << qSetFieldWidth (45) << left << i.value().args
                           << qSetFieldWidth (0) << i.value().help << endl;
                }
            }
            else if (params[0] == "show") {
                if (!resourceSet) {
                    qCritical("%s failed!", qPrintable(params[0]));
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
            else if (params[0] == "acquire") {
                start_timer();
                if (!resourceSet || !resourceSet->acquire()) {
                    stop_timer();
                    qCritical("%s failed!", qPrintable(params[0]));
                }
            }
            else if (params[0] == "release") {
                start_timer();
                if (!resourceSet || !resourceSet->release()) {
                    stop_timer();
                    qCritical("%s failed!", qPrintable(params[0]));
                }
            }
            else if (params[0] == "add") {
                if (!resourceSet) {
                    qCritical("%s failed!", qPrintable(params[0]));
                }
                else if (params.count() < 2 || params[1].isEmpty() || params[1].isNull()) {
                    output << "List of desired resources is missing. Use help.";
                }
                else {
                    bool optional=false;
                    QSet<ResourcePolicy::ResourceType> resToAdd;
                    CommandLineParser::parseResourceList(params[1], resToAdd);
                    if ((params.count() > 2) && (params[2] == "-o")) {
                        optional = true;
                    }
                    foreach (ResourcePolicy::ResourceType resource, resToAdd) {
                        if (!resourceSet->contains(resource)) {
                            resourceSet->addResource(resource);
                        }
                        if (optional) {
                            resourceSet->resource(resource)->setOptional();
                        }
                    }
                }
            }
            else if (params[0] == "remove") {
                if (!resourceSet || params.count() == 1) {
                    qCritical("%s failed!", qPrintable(params[0]));
                }
                else if (params.count() == 1 || params[1].isEmpty() || params[1].isNull()) {
                    output << "List of desired resources is missing. Use Help." << endl;
                }
                else {
                    QSet<ResourcePolicy::ResourceType> resToRemove;
                    CommandLineParser::parseResourceList(params[1], resToRemove);
                    if (params.count() > 2 && params[2] == "-o") {
                        foreach (ResourcePolicy::ResourceType resource, resToRemove) {
                            resourceSet->resource(resource)->setOptional(false);
                        }
                    }
                    else {
                        foreach (ResourcePolicy::ResourceType resource, resToRemove) {
                            resourceSet->deleteResource(resource);
                        }
                    }
                }
            }
            else if (params[0] == "update") {
                if (!resourceSet || !resourceSet->update()) {
                    qCritical("%s failed!", qPrintable(params[0]));
                }
            }
            else if (params[0] == "audio") {
                if (params.count() < 3) {
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
                        if (params[1] == "group") {
                            audioResource->setAudioGroup(params[2]);
                        }
                        else if (params[1] == "pid") {
                            bool ok=false;
                            quint32 pid = (quint32)params[2].toInt(&ok, 10);
                            if (ok && pid != 0) {
                                qDebug("Setting audio PID to %u", pid);
                                audioResource->setProcessID(pid);
                            }
                            else {
                                output << "Bad pid parameter!" << endl;
                            }
                        }
                        else if (params[1] == "tag") {
                            if (params.count() < 4) {
                                output << "tag requires 2 parameters name and value. See help"
                                       << endl;
                            }
                            else {
                                audioResource->setStreamTag(params[2], params[3]);
                            }
                        }
                        else {
                            output << "Unknown audio command!";
                        }
                    }
                }
            }
            else if (params[0] == "addaudio") {
                if (params.count() < 5) {
                    output << "Not enough parameters! See help!" << endl;
                }
                else {
                    AudioResource *audioResource = new AudioResource(params[1]);
                    if (audioResource == NULL) {
                        output << "Failed to create an AudioResource object!" << endl;
                    }
                    else {
                        bool ok=false;
                        quint32 pid = (quint32)params[2].toInt(&ok, 10);
                        if (ok && pid != 0) {
                            audioResource->setProcessID(pid);
                        }
                        else {
                            output << "Bad pid parameter!" << endl;
                        }
                        audioResource->setStreamTag(params[3], params[4]);
                        resourceSet->addResourceObject(audioResource);
                    }
                }
            }
            else if (params[0] == "free") {
                delete resourceSet;
                resourceSet = new ResourceSet(applicationClass);
            }
            else if (!params[0].isEmpty()) {
                QByteArray ba = line.toLatin1();
                const char *c_line = ba.data();
                output << "unknown command '" << c_line << "'" << endl;
            }

            if (!quitFlag) {
                showPrompt();
            }
        }
    }
}

