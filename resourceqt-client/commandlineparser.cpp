/*************************************************************************
This file is part of libresourceqt

Copyright (C) 2011 Nokia Corporation.

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

#include <QDebug>
#include "commandlineparser.h"

#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
#define ENDL endl
#define SKIPEP QString::SkipEmptyParts
#else
#define ENDL Qt::endl
#define SKIPEP Qt::SkipEmptyParts
#endif

extern bool verbose;

QHash<QString, ResourcePolicy::ResourceType> CommandLineParser::resourceValues;

CommandLineParser::CommandLineParser():
        allResources(), optResources(), autoRelease(false), alwaysReply(false),
        verbose(false), allowUnkownResourceClass(false), output(stdout), prefix(""),
        timings(false)
{
    resourceValues["AudioPlayback"] = ResourcePolicy::AudioPlaybackType;
    resourceValues["VideoPlayback"] = ResourcePolicy::VideoPlaybackType;
    resourceValues["AudioRecording"] = ResourcePolicy::AudioRecorderType;
    resourceValues["VideoRecording"] = ResourcePolicy::VideoRecorderType;
    resourceValues["Vibra"] = ResourcePolicy::VibraType;
    resourceValues["Leds"] = ResourcePolicy::LedsType;
    resourceValues["Backlight"] = ResourcePolicy::BacklightType;
    resourceValues["SystemButton"] = ResourcePolicy::SystemButtonType;
    resourceValues["LockButton"] = ResourcePolicy::LockButtonType;
    resourceValues["ScaleButton"] = ResourcePolicy::ScaleButtonType;
    resourceValues["SnapButton"] = ResourcePolicy::SnapButtonType;
    resourceValues["LensCover"] = ResourcePolicy::LensCoverType;
    resourceValues["HeadsetButtons"] = ResourcePolicy::HeadsetButtonsType;
    resourceValues["RearFlashlight"] = ResourcePolicy::RearFlashlightType;
}

CommandLineParser::~CommandLineParser()
{
}

bool CommandLineParser::parseArguments()
{
    QStringList args = QCoreApplication::arguments();
    QStringList::const_iterator ci = args.constBegin();
    // skip the first arg, which is program name
    ++ci;
    while (ci != args.constEnd()) {
        if ((*ci).at(0) == QChar('-')) {
            switch ((*ci).at(1).toLatin1()) {
            case 'd':
                ++ci;
            case 'm':
            case 's':
            case 'i':
                timings = true;
                break;
            case 't':
                break;
            case 'f':
                if (!parseModeValues(*(++ci))) {
                    return false;
                }
                break;
            case 'o':
                if (!parseResourceList(*(++ci), optResources)) {
                    output << "Failed to parse resources: " << *ci << ENDL;
                    return false;
                }
                break;
            case 'u':
                allowUnkownResourceClass = true;
                break;
            case 'v':
                verbose = true;
                break;
            case 'p':
                parsePrefix(*(++ci));
                break;
            case 'h':
            default:
                usage();
                return false;
            }
        }
        else {
            //assume there are no more args
            break;
        }
        ++ci;
    }

    if (ci == args.constEnd()) {
        usage();
        return false;
    }
    else {
        if (!parseClassString(*ci)) {
            return false;
        }
        ++ci;
        if (ci != args.constEnd()) {
            if (!parseResourceList(*ci, allResources)) {
                output << "Error while parsing resource list" << ENDL;
            }
        }
    }

    if (!allResources.contains(optResources)) {
        output << "optional resources are not subset of all resources" << ENDL;
        return false;
    }
    return true;
}

bool CommandLineParser::parseClassString(const QString &str)
{
    if (!allowUnkownResourceClass &&
            (str != "call") &&
            (str != "camera") &&
            (str != "ringtone") &&
            (str != "alarm") &&
            (str != "navigator") &&
            (str != "game") &&
            (str != "player") &&
            (str != "event") &&
            (str != "background") &&
            (str != "videoeditor")) {
        output << "invalid class " << str;
        return false;
    }

    applicationClass = str;
    return true;
}

void CommandLineParser::parsePrefix(const QString &str)
{
    prefix = str + ": ";
}

QString CommandLineParser::getPrefix() const
{
    return prefix;
}

bool CommandLineParser::parseResourceList(const QString &resourceListStr,
        QSet<ResourcePolicy::ResourceType> &resources)
{
    if (resourceListStr.isEmpty() || resourceListStr.isNull()) {
        return false;
    }
    else {
        QStringList resList = resourceListStr.split(",", SKIPEP);

        foreach(QString res, resList) {
            if (resourceValues.contains(res)) {
                resources.insert(resourceValues[res]);
            }
        }
    }

    return true;
}

bool CommandLineParser::parseModeValues(const QString &modeListStr)
{
    if (modeListStr.isEmpty() || modeListStr.isNull()) {
        return false;
    }

    QStringList modeList = modeListStr.split(",", SKIPEP);

    foreach(QString mode, modeList) {
        if (mode == "AutoRelease") {
            autoRelease = true;
        }
        else if (mode == "AlwaysReply") {
            alwaysReply = true;
        }
        else {
            output << "Ignoring unknown mode '" << mode << "'!" << ENDL;
        }
    }
    return true;
}

void CommandLineParser::usage()
{
    output << "usage: resourceqt-client [-h] [-f mode-values]" <<
    "[-o optional-resources] [-i] [-v] [-p prefix] " <<
    "class all-resources" << ENDL;
    output << "\toptions:" << ENDL;
    output << "\t  h\tprint this help message and exit" << ENDL;
    output << "\t i\tshow timings of requests" << ENDL;
    output << "\t v\tshow debug of libresourceqt" << ENDL;
    output << "\t p\tPrefix all output with the given prefix" << ENDL;
    output << "\t  f\tmode values. See 'modes' below for the "
    "\n\t\tsyntax of <mode-values>" << ENDL;
    output << "\t  o\toptional resources. See 'resources' below for the "
    "syntax of\n\t\t<optional-resources>" << ENDL;
    output << "\tclass:" << ENDL;
    output << "\t\tcall\t  - for native or 3rd party telephony" << ENDL;
    output << "\t\tcamera\t  - for photo applications" << ENDL;
    output << "\t\tringtone  - for ringtones" << ENDL;
    output << "\t\talarm\t  - for alarm clock" << ENDL;
    output << "\t\tnavigator - for mapping applications" << ENDL;
    output << "\t\tgame\t  - for gaming" << ENDL;
    output << "\t\tplayer\t  - for media playback/recording" << ENDL;
    output << "\t\tevent\t  - for messaging and other event notifications" << ENDL;
    output << "\t\tbackground - for thumbnailing etc" << ENDL;
    output << "\t\tvideoeditor\t  - for video editing/MMS" << ENDL;
    output << "\tresources:" << ENDL;
    output << "\t  comma separated list of the following resources" << ENDL;

    QHash<QString, ResourcePolicy::ResourceType>::const_iterator ci = resourceValues.constBegin();
    while (ci != resourceValues.constEnd()) {
        output << "\t\t" << ci.key() << ENDL;
        ++ci;
    }
    output << "\t  no whitespace allowed in the resource list." << ENDL;
    output << "\tmodes:" << ENDL;
    output << "\t  comma separated list of the following modes" << ENDL;
    output << "\t\tAutoRelease" << ENDL;
    output << "\t\tAlwaysReply" << ENDL;
}

const QSet<ResourcePolicy::ResourceType>& CommandLineParser::resources() const
{
    return allResources;
}

const QSet<ResourcePolicy::ResourceType>& CommandLineParser::optionalResources() const
{
    return optResources;
}

QString CommandLineParser::resourceApplicationClass() const
{
    return applicationClass;
}

bool CommandLineParser::shouldAutoRelease() const
{
    return autoRelease;
}

bool CommandLineParser::shouldAlwaysReply() const
{
    return alwaysReply;
}

bool CommandLineParser::shouldBeVerbose() const
{
    return verbose;
}

bool CommandLineParser::showTimings() const
{
    return timings;
}
