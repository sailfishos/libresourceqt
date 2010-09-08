#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include "client.h"
#include "commandlineparser.h"

bool verbose = true;

void debugOutput(QtMsgType type, const char *msg)
{
    switch (type) {
    case QtDebugMsg:
        if (verbose)
            fprintf(stderr, "Debug: %s\n", msg);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s\n", msg);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s\n", msg);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal Error: %s\n", msg);
        abort();
    }
}

int main(int argc, char *argv[])
{
    qInstallMsgHandler(debugOutput);
    QCoreApplication app(argc, argv);
    CommandLineParser parser;
    Client client;

    if (!parser.parseArguments()) {
        return 1;
    }

    if (!client.initialize(parser)) {
        qFatal("Initialization failed");
    }

    return app.exec();
}

