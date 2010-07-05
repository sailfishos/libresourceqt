#include <QtGui>
#include <MApplication>
#include <MApplicationWindow>
#include "playerpage.h"

int main (int argc, char *argv[])
{
    MApplication app(argc, argv);
    app.setApplicationName("Policy-Aware Player");
    app.setOrganizationName("Nokia");
    app.setQuitOnLastWindowClosed(true);

    MApplicationWindow window;
    window.show();

    PlayerPage *player = new PlayerPage();
    player->setTitle("Policy-Aware Player");
    player->appear(&window);

    return app.exec();
}

