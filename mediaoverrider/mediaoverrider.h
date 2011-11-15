#include <MApplicationPage>
#include <MButton>
#include <MLabel>
#include <MGridLayoutPolicy>
#include <MLayout>
#include <override.h>

class MediaOverrider : public MApplicationPage
{
    Q_OBJECT

public:
    MediaOverrider();
    ~MediaOverrider();

    private:
        ResourcePolicy::Override *mute;
        ResourcePolicy::Override *privacyOverride;
        ResourcePolicy::Override *btOverride;

        MLayout *layout;
        MGridLayoutPolicy *policy;
        MButton *muteButton;
        MButton *privacyButton;
        MButton *btButton;

  private slots:
         void handleMuteChange(bool newState);
         void handlePrivacyChange(bool newState);
         void handleBtChange(bool newState);
};
