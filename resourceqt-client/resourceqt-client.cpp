#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <stdlib.h>
#include <errno.h>
#include <libgen.h>
#include <getopt.h>
#include "client.h"

class CommandLineParser
{
public:
    CommandLineParser(int argc, char** argv) {
        exitCode	= 0;
        exitFlag 	= false;
        exeName = strdup(basename(argv[0]));

        resourcesAll 		= 0;
        resourcesOptional	= 0;
        autoRelease = false;
        alwaysReply = false;

        parseArguments(argc, argv);
    }

    ~CommandLineParser() {
        if (exeName != NULL) {
            delete exeName;
            exeName = NULL;
        }
    }

public:
    bool		exitFlag;
    int			exitCode;

    uint32_t	resourcesAll;
    uint32_t	resourcesOptional;
    QString		applicationClass;
    bool		autoRelease;
    bool		alwaysReply;

    void printMessage(const char *fmt, ...) {
        va_list  ap;
        char     fmtbuf[512];

        snprintf(fmtbuf, sizeof(fmtbuf), "%s\n", fmt);
        va_start(ap, fmt);
        vprintf(fmtbuf, ap);
        va_end(ap);
    }

private:
    char* 	exeName;

    void printError(const char *fmt, ...) {
        va_list  ap;
        char     fmtbuf[512];

        snprintf(fmtbuf, sizeof(fmtbuf), "%s\n", fmt);
        va_start(ap, fmt);
        vprintf(fmtbuf, ap);
        va_end(ap);

        exitCode = errno;
        exitFlag = true;
    }

    void parseArguments(int argc, char** argv) {
        int   option;

        while ((option = getopt(argc, argv, "hm:o:")) != -1) {
            switch (option) {
            case 'h':
                usage(0);
                return;
            case 'm':
                parseModeValues(optarg);
                break;
            case 'o':
                resourcesOptional = Client::parseResourceList(optarg);
                break;
            default:
                usage(EINVAL);
                return;
            }
        }

        if ((optind != argc - 2) && (optind != argc - 1)) {
            usage(EINVAL);
            return;
        }
        else {
            applicationClass = parseClassString(argv[optind]);
            if (argc > optind + 1)
                resourcesAll = Client::parseResourceList(argv[optind+1]);
            else
                resourcesAll = 0;
        }

        if (!resourcesAll) {
            printMessage("No resources found, use add command to add some ...");
        }

        if ((resourcesAll | resourcesOptional) != resourcesAll) {
            printError("optional resources are not subset of all resources");
        }
    }

    char* parseClassString(char *str) {
        if (strcmp(str, "call") &&
                strcmp(str, "camera") &&
                strcmp(str, "ringtone") &&
                strcmp(str, "alarm") &&
                strcmp(str, "navigator") &&
                strcmp(str, "game") &&
                strcmp(str, "player") &&
                strcmp(str, "event") &&
                strcmp(str, "background")) {
            printError("invalid class '%s'", str);
            return NULL;
        }

        return str;
    }

    void parseModeValues(QString modeListStr) {
        if (!modeListStr.isEmpty() && !modeListStr.isNull()) {
            QStringList modeList = modeListStr.split(",", QString::SkipEmptyParts);

            for (int i = 0; i < modeList.count(); i++) {
                if (modeList[i] == "AutoRelease") {
                    autoRelease = true;
                }
                else if (modeList[i] == "AlwaysReply") {
                    alwaysReply = true;
                }
                else {
                    const char* mode = qPrintable(modeList[i]);
                    printMessage("Ignoring unknown mode '%s'!", mode);
                }
            }
        }
    }

    void usage(int theExitCode) {
        printf("usage: %s [-h] [-t] [-v] [-m mode-values]"
               "[-o optional-resources] [-s shared-resources -m shared-mask] "
               "class all-resources\n",
               exeName);
        printf("\toptions:\n");
        printf("\t  h\tprint this help message and exit\n");
        printf("\t  m\tmode values. See 'modes' below for the "
               "\n\t\tsyntax of <mode-values>\n");
        printf("\t  o\toptional resources. See 'resources' below for the "
               "syntax of\n\t\t<optional-resources>\n");
        printf("\tclass:\n");
        printf("\t\tcall\t  - for native or 3rd party telephony\n");
        printf("\t\tcamera\t  - for photo applications\n");
        printf("\t\tringtone  - for ringtones\n");
        printf("\t\talarm\t  - for alarm clock\n");
        printf("\t\tnavigator - for mapping applications\n");
        printf("\t\tgame\t  - for gaming\n");
        printf("\t\tplayer\t  - for media playback/recording\n");
        printf("\t\tevent\t  - for messaging and other event notifications\n");
        printf("\t\tbackground - for thumbnailing etc\n");
        printf("\tresources:\n");
        printf("\t  comma separated list of the following resources\n");
        printf("\t\tAudioPlayback\n");
        printf("\t\tVideoPlayback\n");
        printf("\t\tAudioRecording\n");
        printf("\t\tVideoRecording\n");
        printf("\t\tVibra\n");
        printf("\t\tLeds\n");
        printf("\t\tBackLight\n");
        printf("\t\tSystemButton\n");
        printf("\t\tLockButton\n");
        printf("\t\tScaleButton\n");
        printf("\t\tSnapButton\n");
        printf("\t\tLensCover\n");
        printf("\t  no whitespace allowed in the resource list.\n");
        printf("\tmodes:\n");
        printf("\t  comma separated list of the following modes\n");
        printf("\t\tAutoRelease\n");
        printf("\t\tAlwaysReply\n");
        fflush(stdout);

        exitCode = theExitCode;
        exitFlag = true;
    }
};

int main(int argc, char *argv[])
{
    CommandLineParser parser(argc, argv);

    if (parser.exitFlag)
        return parser.exitCode;

    QCoreApplication app(argc, argv);

    Client client(parser.applicationClass);
    if (!client.initialize(parser.resourcesAll, parser.resourcesOptional, parser.alwaysReply, parser.autoRelease)) {
        parser.printMessage("initialization failed!");
        return EINVAL;
    }

    return app.exec();
}
