#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <stdlib.h>
#include <errno.h>
#include <libgen.h>
#include <getopt.h>
#include "client.h"

class ConfigParser
{
public:
    ConfigParser()
    {
        exitCode = 0;
        exitFlag = false;

        resourcesAll = 0;
        resourcesOptional = 0;
        repeatCount = 100;

        parseConfig();
    }

public:
    bool exitFlag;
    int exitCode;

    int      repeatCount;
    uint32_t resourcesAll;
    uint32_t resourcesOptional;
    double   timeLimit;
    QString applicationClass;

private:
    void parseConfig()
    {
        QSettings settings("/usr/lib/libresourceqt-performance-tests/test-resourceqt-performance.ini", QSettings::IniFormat);

        settings.beginGroup("config");
        QString resAll = settings.value("AllResources", "AudioPlayback,VideoPlayback").toString();
        QString resOpt = settings.value("OptionalResources", "").toString();
        QString appClass = settings.value("ApplicationClass", "player").toString().toLower();
        repeatCount = settings.value("TestCount", 100).toInt();
        settings.endGroup();

        settings.beginGroup("main");
        timeLimit = settings.value("limit", 80.0).toDouble();
        settings.endGroup();

        resourcesAll = Client::parseResourceList(resAll);
        resourcesOptional = Client::parseResourceList(resOpt);
        applicationClass = parseClassString(qPrintable(appClass));

        if( applicationClass.isEmpty() )
        {
            printf("Wrong application class in configuration file! Using default ...\n");
            applicationClass = "player";
        }

        if( !resourcesAll )
        {
            resAll = "AudioPlayback,VideoPlayback";
            resourcesAll = Client::parseResourceList(resAll);

            printf("No resources found in configuration file! Using defaults ...\n");
        }

        if( (resourcesAll | resourcesOptional) != resourcesAll )
        {
            printf("Optional resources are not subset of all resources!\n");
            exitFlag = true;
            exitCode = 1;
        }
    }

    const char* parseClassString(const char *str)
    {
        if( strcmp(str, "call") && strcmp(str, "camera") && strcmp(str, "ringtone") && strcmp(str, "alarm") && strcmp(str, "navigator") && strcmp(str, "game")
                && strcmp(str, "player") && strcmp(str, "event") && strcmp(str, "background") )
        {
            printf("Invalid class '%s'", str);
            return NULL;
        }

        return str;
    }
};

int main(int argc, char *argv[])
{
    ConfigParser parser;

    if( parser.exitFlag )
        return parser.exitCode;

    QCoreApplication app(argc, argv);
    Client client(parser.repeatCount, parser.applicationClass, parser.resourcesAll, parser.resourcesOptional);
    app.exec();

    int retval = client.getFailed();
    if( retval > 0 )
        printf("One or more test iterations failed!\n");

    int oldval = retval;
    retval += client.getAcquireAverage() > parser.timeLimit ? 100 : 0;
    if( retval > oldval )
        printf("Acquire average time is higher than limit (%.2f/%.2f ms) !\n", client.getAcquireAverage(), parser.timeLimit);

    oldval = retval;
    retval += client.getReleaseAverage() > parser.timeLimit ? 100 : 0;
    if( retval > oldval )
        printf("Release average time is higher than limit (%.2f/%.2f ms) !\n", client.getReleaseAverage(), parser.timeLimit);

    return retval;
}
