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
  : MApplicationPage(), labelsVisible(true) {
}

/**
  * Creates a menu action to enable or disable "policy awareness", layouts,
  * the player widget (our own subclass of MVideoWidget) and labels that show
  * the file name and playback position.
  *
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

  // video widget
  Streamer *streamer = new Streamer(this);
  playerWidget = new PlayerWidget(streamer);
//  mediaPlayer->setVideoOutput(playerWidget);

  // height is hard-coded for a lack of a better option
//  playerWidget->setMaximumSize(1000, 300);

  connect(playerWidget, SIGNAL(playerPositionChanged()), this, SLOT(onPositionChanged()));
  connect(playerWidget, SIGNAL(playing()), this, SLOT(setPlayingIcon()));
  connect(playerWidget, SIGNAL(paused()),  this, SLOT(setPausedIcon()));
  connect(playerWidget, SIGNAL(denied()),  this, SLOT(handleDenied()));

  // labels
  lblTitle = new MLabel("Tap audio or video button to load a file", this);
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

  QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget(this);
//  proxy->setWidget(playerWidget);

  layoutPolicy->addItem(lblTitle,    Qt::AlignVCenter);
  layoutPolicy->addItem(proxy);
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
  * scrub through media content, and two buttons to open audio and video files.
  */
void PlayerPage::makeControlBar(MLinearLayoutPolicy *controlBarPolicy) {

  btnPlay = makeButton("icon-m-common-play", false);

  seekbar = new MSeekBar();
  controlBarPolicy->addItem(seekbar, Qt::AlignVCenter);
  setEnabled(seekbar, false);

  btnLoadAudio = makeButton("icon-m-content-audio");
//  btnLoadVideo = makeButton("icon-m-common-video");

  connect(btnLoadAudio, SIGNAL(clicked()), this, SLOT(openAudioFile()));
//  connect(btnLoadVideo, SIGNAL(clicked()), this, SLOT(openVideoFile()));
  connect(btnPlay, SIGNAL(clicked()), this, SLOT(playOrPause()));

  connect(seekbar, SIGNAL(sliderPressed()),  this, SLOT(sliderPressed()));
  connect(seekbar, SIGNAL(sliderReleased()), this, SLOT(seekToNewPosition()));

  seekbar->setValue(0);
}

/**
  * Shows/hides labels depending on orientation.
  *
  * \see PlayerPage::updateLabelsVisibility()
  */
void PlayerPage::orientationChangeEvent(MOrientationChangeEvent */*event*/) {
  if (isContentCreated())  updateLabelsVisibility();
}

/**
  * Shows/hides labels depending on orientation.  For audio files we show labels
  * at all times, and for video playback we hide the labels in landscape orientation,
  * since most of the space is occupied by the player widget.
  *
  */
void PlayerPage::updateLabelsVisibility() {
  if (sceneManager()->orientation() == M::Portrait || playerWidget->filetype == PlayerWidget::AUDIO) {
    if (!labelsVisible) {
      layoutPolicy->insertItem(0, lblTitle, Qt::AlignVCenter);
      layoutPolicy->insertItem(2, lblPosition, Qt::AlignVCenter);
      labelsVisible = true;
    }
  } else if (playerWidget->filetype == PlayerWidget::VIDEO) {
    if (labelsVisible) {
      layoutPolicy->removeItem(lblTitle);
      layoutPolicy->removeItem(lblPosition);
      labelsVisible = false;
    }
  }
}

/**
  * Opens the file open dialog on the .sounds directory of the user,
  * sets the "filetype" variable to AUDIO.
  *
  * \see PlayerPage::openFile()
  */
void PlayerPage::openAudioFile() {

  setPausedIcon();
  playerWidget->stop();

  playerWidget->filetype = PlayerWidget::AUDIO;
  openFile("/home/user/MyDocs/.sounds/");

  seekbar->setRange(0, playerWidget->length() > 0 ? playerWidget->length() : 10);
  seekbar->setValue(0);

  //playerWidget->setVisible(false);
}

/**
  * Opens the file open dialog on the .videos directory of the user,
  * sets the "filetype" variable to VIDEO.
  *
  * \see PlayerPage::openFile()
  */
void PlayerPage::openVideoFile() {
  playerWidget->filetype = PlayerWidget::VIDEO;
  openFile("/home/user/MyDocs/.videos/");

//  playerWidget->setVisible(true);
}

/**
  * Opens the file open dialog, and then loads the file selected.
  * Helper function for PlayerPage::openAudioFile() and PlayerPage::openVideoFile().
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
    updateLabelsVisibility();
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
