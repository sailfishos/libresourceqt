#include "resourceoverrider.h"

ResourceOverrider::ResourceOverrider(int argc, char **argv, QObject *parent):
    QObject(parent), app(argc, argv)
{
//    MTheme::loadCSS("resourceoverrider.css");
    window = new MApplicationWindow;
    page = new MApplicationPage;

    layout = new MLayout;
    policy = new MGridLayoutPolicy(layout);
    policy->setSpacing(10);
    
    page->setTitle("Resource Overrider");
    MLabel * label = new MLabel("Press the toggle buttons to change overrides");
    policy->addItem(label, 0, 1);
    label->setObjectName("label");
    label->setAlignment(Qt::AlignCenter);

    muteButton = new MButton(page->centralWidget());
    muteButton->setText("Mute");
    muteButton->setViewType(MButton::toggleType);
    muteButton->setCheckable(true);
    policy->addItem(muteButton, 1,0);
    muteButton->setObjectName("button");

    privacyButton = new MButton(page->centralWidget());
    privacyButton->setText("Privacy");
    privacyButton->setViewType(MButton::toggleType);
    privacyButton->setCheckable(true);
    policy->addItem(privacyButton, 1, 1);
    privacyButton->setObjectName("button");

    btButton = new MButton(page->centralWidget());
    btButton->setText("BT");
    btButton->setViewType(MButton::toggleType);
    btButton->setCheckable(true);
    policy->addItem(btButton, 1, 2);
    btButton->setObjectName("button");

    page->centralWidget()->setLayout(layout);

    mute = ResourcePolicy::createMute(this);
    privacyOverride = ResourcePolicy::createPrivacyOverride(this);
    btOverride = ResourcePolicy::createBluetoothOVerride(this);

    QObject::connect(mute, SIGNAL(changed(bool)), this, SLOT(handleMuteChange(bool)));
    QObject::connect(muteButton, SIGNAL(toggled(bool)), mute, SLOT(request(bool)));

    QObject::connect(privacyOverride, SIGNAL(changed(bool)), this, SLOT(handlePrivacyChange(bool)));
    QObject::connect(privacyButton, SIGNAL(toggled(bool)), privacyOverride, SLOT(request(bool)));

    QObject::connect(btOverride, SIGNAL(changed(bool)), this, SLOT(handleBtChange(bool)));
    QObject::connect(btButton, SIGNAL(toggled(bool)), btOverride, SLOT(request(bool)));

}

ResourceOverrider::~ResourceOverrider()
{}

int ResourceOverrider::run()
{
    page->appear();
    window->show();

    return app.exec();
}

void ResourceOverrider::handleMuteChange(bool newState)
{
    muteButton->setChecked(newState);
}

void ResourceOverrider::handlePrivacyChange(bool newState)
{
    privacyButton->setChecked(newState);
}

void ResourceOverrider::handleBtChange(bool newState)
{
    btButton->setChecked(newState);
}

