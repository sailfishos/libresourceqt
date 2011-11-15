#include <QtGui>
#include <MApplication>
#include <MApplicationWindow>
#include "mediaoverrider.h"

int main (int argc, char *argv[])
{
    MApplication app(argc, argv);
    app.setApplicationName("Media Overrider");
    app.setOrganizationName("Nokia");
    app.setQuitOnLastWindowClosed(true);

    MApplicationWindow window;
    window.show();

    MediaOverrider *over = new MediaOverrider();
    over->setTitle("Media Overrider");
    over->appear(&window);

    return app.exec();
}
