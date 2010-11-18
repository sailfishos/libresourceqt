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
#include <QtCore/QString>
#include <QtCore/QStringList>
#include "client.h"
#include "commandlineparser.h"

bool verbose = false;

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
    verbose = parser.shouldBeVerbose();

    if (!client.initialize(parser)) {
        qFatal("Initialization failed");
    }

    return app.exec();
}

