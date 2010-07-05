#include <MApplication>
#include <MApplicationWindow>
#include <MApplicationPage>
#include <MButton>
#include <MLabel>
#include <MGridLayoutPolicy>
#include <MLayout>
#include <MTheme>

#include <override.h>

class ResourceOverrider: public QObject
{
    Q_OBJECT
public:
    ResourceOverrider(int argc, char **argv, QObject *parent=NULL);
    ~ResourceOverrider();
    int run();
private slots:
    void handleMuteChange(bool newState);
    void handlePrivacyChange(bool newState);
    void handleBtChange(bool newState);

private:
    MApplication app;
    ResourcePolicy::Override *mute;
    ResourcePolicy::Override *privacyOverride;
    ResourcePolicy::Override *btOverride;

    MApplicationWindow *window;
    MApplicationPage *page;
    MLayout *layout;
    MGridLayoutPolicy *policy;
    MButton *muteButton;
    MButton *privacyButton;
    MButton *btButton;
};

