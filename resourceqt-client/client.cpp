#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>

#include "client.h"

using namespace ResourcePolicy;

Client::Client()
    : QObject(), standardInput(stdin, QFile::ReadOnly), applicationClass(), resourceSet(NULL)
{
    mainTimerID   = startTimer(0);
}

Client::~Client()
{
    killTimer(mainTimerID);

    delete resourceSet;
}

void Client::showPrompt()
{
    printf("res-client> ");
    fflush(stdout);
}

bool Client::initialize(const CommandLineParser &parser)
{
	QSet<ResourcePolicy::ResourceType> allResources;
	QSet<ResourcePolicy::ResourceType> optionalResources;
    resourceSet = new ResourceSet(parser.resourceApplicationClass());
    if (resourceSet == NULL) {
        return false;
    }

    if (parser.shouldAlwaysReply()) {
        qDebug("client: AlwaysReply");
        resourceSet->setAlwaysReply();
    }

    if (parser.shouldAutoRelease()) {
        qDebug("client: AutoRelease");
        resourceSet->setAutoRelease();
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

void Client::showResources(const QList<ResourceType> resList)
{
    foreach (ResourceType resource, resList) {
        printf("\t%s\n", resourceTypeToString(resource));
    }
}

void Client::showResources(const QList<Resource*> resList)
{
    foreach (Resource* resource, resList) {
        printf("\t%s%s%s\n", resourceTypeToString(resource->type()),
               resource->isOptional() ? " (optional)" : "",
               resource->isGranted() ? " (granted)" : "");
    }
}

void Client::resourceAcquiredHandler(const QList<ResourceType>& /*grantedResList*/)
{
    double ms = stop_timer();
    if( ms > 0.0 ) {
        printf("Operation took %.2f ms\n", ms);
    }

    QList<Resource*> list = resourceSet->resources();
    if (!list.count()) {
        printf("\nGranted resource set is empty. Possible bug?\n");
    }
    else {
        printf("\nManager grants access to these resources:\n");
        printf("Resource set:\n");
        showResources(list);
    }
    showPrompt();
}

void Client::resourceDeniedHandler()
{
    double ms = stop_timer();
    if( ms > 0.0 ) {
        printf("Operation took %.2f ms\n", ms);
    }

    printf("\nManager denies access to resources!\n");
    showPrompt();
}

void Client::resourceLostHandler()
{
    double ms = stop_timer();
    if( ms > 0.0 ) {
        printf("Operation took %.2f ms\n", ms);
    }

    printf("\nLost resources from manager!\n");
    showPrompt();
}

void Client::resourceReleasedHandler()
{
    double ms = stop_timer();
    if( ms > 0.0 ) {
        printf("Operation took %.2f ms\n", ms);
    }

    printf("\nAll resources released\n");
    showPrompt();
}

void Client::resourcesBecameAvailableHandler(const QList<ResourcePolicy::ResourceType> &availableResources)
{
    printf("\nManager advice: These resources are available:\n");
    printf("Resource set:\n");
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
                printf("Available commands:\n");
                printf("\t  help   \tprint this help message\n");
                printf("\t  quit   \texit application\n");
                printf("\t  free   \tdestroy and free the resources\n");
                printf("\t  acquire\tacquire required resources\n");
                printf("\t  release\trelease resources\n");
                printf("\t  update\tupdate modified resource set after add or remove command\n");
                printf("\t  add reslist [-o]\tadd reosurce list, if -o provided, set as optional\n");
                printf("\t  remove reslist [-o]\tremove reosurce list, if -o provided, removed only optional flag\n");
                printf("\t  audio pid <pid> | group <audio group> | tag <name> <value>\tset audio properties\n");
                printf("\t  addaudio <audio group> <pid> <tag name> <tag value>\n");
                printf("\t  show  \tshow resources\n");
            }
            else if (params[0] == "show") {
                if (!resourceSet) {
                    printf("Show failed!\n");
                }
                else {
                    QList<Resource*> list = resourceSet->resources();
                    if (!list.count()) {
                        printf("Resource set is empty, use add command to add some ...\n");
                    }
                    else {
                        printf("Resource set:\n");
                        showResources(list);
                    }
                }
            }
            else if (params[0] == "acquire") {
                start_timer();
                if (!resourceSet || !resourceSet->acquire()) {
                    double ms = stop_timer();
                    if( ms > 0.0 ) {
                        printf("Operation took %.2f ms\n", ms);
                    }

                    printf("Acquire failed!\n");
                }
            }
            else if (params[0] == "release") {
                start_timer();
                if (!resourceSet || !resourceSet->release()) {
                    double ms = stop_timer();
                    if( ms > 0.0 ) {
                        printf("Operation took %.2f ms\n", ms);
                    }

                    printf("Release failed!\n");
                }
            }
            else if (params[0] == "add") {
                if (!resourceSet) {
                    printf("Add failed!\n");
                }
                else if (params.count() == 1 || params[1].isEmpty() || params[1].isNull()) {
                    printf("List of desired resources is missing. See help ...\n");
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
                    printf("Remove failed!\n");
                }
                else if (params.count() == 1 || params[1].isEmpty() || params[1].isNull()) {
                    printf("List of desired resources is missing. See help ...\n");
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
                    printf("Update failed!\n");
                }
            }
            else if (params[0] == "audio") {
                if (params.count() < 3) {
                    printf("Not enough parameters! See help!\n");
                }
                else {
                    Resource *resource = resourceSet->resource(AudioPlaybackType);
                    AudioResource *audioResource = static_cast<AudioResource*>(resource);
                    qDebug("resource = %p audioResource = %p", resource, audioResource);
                    if (audioResource == NULL) {
                        printf("No AudioResource available in set!\n");
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
                                printf("Bad pid parameter!\n");
                            }
                        }
                        else if (params[1] == "tag") {
                            if (params.count() < 4) {
                                printf("tag requires TWO parameters name and value. See help \n");
                            }
                            else {
                                audioResource->setStreamTag(params[2], params[3]);
                            }
                        }
                        else {
                            printf("Unknown audio command!\n");
                        }
                    }
                }
            }
            else if (params[0] == "addaudio") {
                if (params.count() < 5) {
                    printf("Not enough parameters! See help!\n");
                }
                else {
                    AudioResource *audioResource = new AudioResource(params[1]);
                    if (audioResource == NULL) {
                        printf("Failed to create an AudioResource object!\n");
                    }
                    else {
                        bool ok=false;
                        quint32 pid = (quint32)params[2].toInt(&ok, 10);
                        if (ok && pid != 0) {
                            audioResource->setProcessID(pid);
                        }
                        else {
                            printf("Bad pid parameter!\n");
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
                printf("unknown command '%s'\n", c_line);
            }

            if (!quitFlag) {
                showPrompt();
            }
        }
    }
}

