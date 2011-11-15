#include "mediaoverrider.h"

MediaOverrider::MediaOverrider()
     : MApplicationPage()
{
    layout = new MLayout();
    policy = new MGridLayoutPolicy(layout);
    policy->setSpacing(20);
    policy->setColumnMinimumWidth 	(  0,  200 );

    MLabel * label = new MLabel("Toggle buttons to change overrides");
    policy->addItem(label, 0, 0);
    label->setObjectName("label");
    label->setAlignment(Qt::AlignCenter);

    muteButton = new MButton(centralWidget());
    muteButton->setText("Mute");
    muteButton->setViewType(MButton::toggleType);
    muteButton->setCheckable(true);
    policy->addItem(muteButton, 1, 0);
    muteButton->setObjectName("button");

    privacyButton = new MButton(centralWidget());
    privacyButton->setText("Privacy");
    privacyButton->setViewType(MButton::toggleType);
    privacyButton->setCheckable(true);
    policy->addItem(privacyButton, 2, 0);
    privacyButton->setObjectName("button");

    btButton = new MButton(centralWidget());
    btButton->setText("BT");
    btButton->setViewType(MButton::toggleType);
    btButton->setCheckable(true);
    policy->addItem(btButton, 3, 0);
    btButton->setObjectName("button");

    centralWidget()->setLayout(layout);

    mute                    = ResourcePolicy::createMute(this);
    privacyOverride = ResourcePolicy::createPrivacyOverride(this);
    btOverride          = ResourcePolicy::createBluetoothOVerride(this);

    QObject::connect(mute, SIGNAL(changed(bool)), this, SLOT(handleMuteChange(bool)));
    QObject::connect(muteButton, SIGNAL(toggled(bool)), mute, SLOT(request(bool)));

    QObject::connect(privacyOverride, SIGNAL(changed(bool)), this, SLOT(handlePrivacyChange(bool)));
    QObject::connect(privacyButton, SIGNAL(toggled(bool)), privacyOverride, SLOT(request(bool)));

    QObject::connect(btOverride, SIGNAL(changed(bool)), this, SLOT(handleBtChange(bool)));
    QObject::connect(btButton, SIGNAL(toggled(bool)), btOverride, SLOT(request(bool)));

}


MediaOverrider::~MediaOverrider(){}


void MediaOverrider::handleMuteChange(bool newState)
{
    muteButton->setChecked(newState);
}


void MediaOverrider::handlePrivacyChange(bool newState)
{
    privacyButton->setChecked(newState);
}


void MediaOverrider::handleBtChange(bool newState)
{
    btButton->setChecked(newState);
}

