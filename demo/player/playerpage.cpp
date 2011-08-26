/*************************************************************************
This file is part of libresourceqt

Copyright (C) 2011 Nokia Corporation.

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

#include "playerpage.h"
#include <cstdio>

PlayerPage::PlayerPage()
  : MApplicationPage() {
}

/**
  * Creates a menu action to enable or disable "policy awareness", layouts,
  * the player widget and labels that show the file name and playback position.
  */
void PlayerPage::createContent() {
  // 1. create menu
  menuAction = new MAction("Make policy-unaware", this);
  menuAction->setLocation(MAction::ApplicationMenuLocation);
  addAction(menuAction);

  connect(menuAction, SIGNAL(triggered()), this, SLOT(togglePolicyAwareness()));

  // 2. create layouts
  MLayout *layout = new MLayout(centralWidget());
  layoutPolicy = new MLinearLayoutPolicy(layout, Qt::Vertical);

  Streamer *streamer = new Streamer(this);
  playerWidget = new PlayerWidget(streamer);

  connect(playerWidget, SIGNAL(playerPositionChanged()), this, SLOT(onPositionChanged()));
  connect(playerWidget, SIGNAL(playing()), this, SLOT(setPlayingIcon()));
  connect(playerWidget, SIGNAL(paused()),  this, SLOT(setPausedIcon()));
  connect(playerWidget, SIGNAL(denied()),  this, SLOT(handleDenied()));

  // labels
  lblTitle = new MLabel("Tap audio button to load a file", this);
  lblTitle->setAlignment(Qt::AlignCenter);
  QFont titleFont;
  titleFont.setPixelSize(20);
  lblTitle->setFont(titleFont);

  lblPosition = new MLabel("", this);
  lblPosition->setAlignment(Qt::AlignCenter);
  QFont positionFont;
  positionFont.setPixelSize(50);
  lblPosition->setFont(positionFont);
  lblPosition->setPreferredSize(QSize(lblPosition->preferredSize().width(), 200));

  // control bar
  MLayout *controlBarLayout = new MLayout(layout);
  controlBarPolicy = new MLinearLayoutPolicy(controlBarLayout, Qt::Horizontal);
  makeControlBar(controlBarPolicy);

  layoutPolicy->addItem(lblTitle,    Qt::AlignVCenter);
  layoutPolicy->addItem(lblPosition, Qt::AlignVCenter);
  layoutPolicy->addStretch();
  layoutPolicy->addItem(controlBarLayout, Qt::AlignBottom);
}


/**
  * A helper function to create icon buttons.
  */
MButton* PlayerPage::makeButton(QString iconID, bool enabled) {
  MButton *button = new MButton("", this);
  button->setViewType(MButton::iconType);
  button->setIconID(iconID);
  controlBarPolicy->addItem(button, Qt::AlignVCenter);
  setEnabled(button, enabled);
  return button;
}

/**
  * A helper function to make widgets appear enabled or disabled.
  */
void PlayerPage::setEnabled(MWidget *widget, bool enabled) {
  // seekbar dissappears completely when disabled on current theme
  if (widget != seekbar)  widget->setEnabled(enabled);
  // and disabled widgets are not highlighted in any way, so we use opacity for that
  widget->setOpacity(enabled ? 1.0 : 0.5);
}

/**
  * Creates a control bar.  It consists of the play/pause button, a seek bar to
  * scrub through media content, and a button to open audio files.
  */
void PlayerPage::makeControlBar(MLinearLayoutPolicy *controlBarPolicy) {
  btnPlay = makeButton("icon-m-common-play", false);

  seekbar = new MSeekBar();
  controlBarPolicy->addItem(seekbar, Qt::AlignVCenter);
  setEnabled(seekbar, false);

  btnLoadAudio = makeButton("icon-m-content-audio");

  connect(btnLoadAudio, SIGNAL(clicked()), this, SLOT(openAudioFile()));
  connect(btnPlay, SIGNAL(clicked()), this, SLOT(playOrPause()));

  connect(seekbar, SIGNAL(sliderPressed()),  this, SLOT(sliderPressed()));
  connect(seekbar, SIGNAL(sliderReleased()), this, SLOT(seekToNewPosition()));

  seekbar->setValue(0);
}

/**
  * Opens the file open dialog on the .sounds directory of the user
  *
  * \see PlayerPage::openFile()
  */
void PlayerPage::openAudioFile() {
  setPausedIcon();
  playerWidget->stop();

  openFile("/home/user/MyDocs/.sounds/");

  seekbar->setRange(0, playerWidget->length() > 0 ? playerWidget->length() : 10);
  seekbar->setValue(0);

}

/**
  * Opens the file open dialog, and then loads the file selected.
  */
void PlayerPage::openFile(QString dir) {
  qDebug("openFile");

  QString fileName = QFileDialog::getOpenFileName(NULL, tr("Open File"),
                                                  dir,
                                                  tr("All Files (*.*)"));

  if (fileName != "") {
    playerWidget->setPosition(0);
    playerWidget->open(fileName);

    QString prettyName = fileName;
    if (prettyName.contains("/"))  prettyName = prettyName.mid(prettyName.lastIndexOf("/")+1);
    lblTitle->setText(prettyName);

    seekbarPressed = false;
    setEnabled(btnPlay, true);
    setEnabled(seekbar, true);
  }  

  seekbar->setValue(0);
}

/**
  * Starts or pauses the playback.
  */
void PlayerPage::playOrPause() {
  if (playerWidget->state() != Streamer::PlayingState) {
    setPlayingIcon();
    playerWidget->play();
  } else {
    setPausedIcon();
    playerWidget->pause();
  }
}

/**
  * Displays the "playing" icon for the play/pause button.
  */
void PlayerPage::setPlayingIcon() {
  btnPlay->setIconID("icon-m-common-pause");
}

/**
  * Displays the "paused" icon for the play/pause button.
  */
void PlayerPage::setPausedIcon() {
  btnPlay->setIconID("icon-m-common-play");
}

/**
  * Sets title text to display info text that playback is not possible at the moment.
  */
void PlayerPage::handleDenied() {
  lblTitle->setText("Not allowed to play at the moment, please wait");
}

/**
  * Updates the label that displays current playback position
  * and the seek bar.  This signal is issued by PlayerWidget every
  * 100 ms.
  *
  * \see PlayerWidget::timerEvent()
  */
void PlayerPage::onPositionChanged() {

  QString pos;
  int mins = playerWidget->position() / 1000.0 / 60;
  int secs = (playerWidget->position() - mins * 60 * 1000.0) / 1000.0;
  pos.sprintf("%02d:%02d", mins, secs);
  lblPosition->setText(pos);

  seekbar->setRange(0, playerWidget->length());
  if (!seekbarPressed)  seekbar->setValue(playerWidget->position());
}

/**
  * Sets our private variable seekbarPressed to true, so that we stop
  * updating the seekbar for the time the control is being used by the user.
  */
void PlayerPage::sliderPressed() {
  seekbarPressed = true;
}

/**
  * Jumps to a new position in the media.
  */
void PlayerPage::seekToNewPosition() {
  seekbarPressed = false;
  playerWidget->seek(seekbar->value());
}

/**
  * A menu-action to put the demo to policy-aware or policy-unaware mode.
  * This means that depending on a flag we will or will not acquire the audio resource
  * prior to beginning to playback audio.  See PlayerWidget for the policy-related functionality.
  */
void PlayerPage::togglePolicyAwareness() {
  playerWidget->setPolicyAware(!playerWidget->policyAware());
  menuAction->setText(playerWidget->policyAware() ? "Make policy-unaware" : "Make policy-aware");
}
