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

#pragma once

#include <QtGui>
#include <QFileDialog>

#include <MApplicationPage>
#include <MVideoWidget>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MButton>
#include <MSlider>
#include <MOrientationChangeEvent>
#include <MApplicationMenu>
#include <MAction>
#include <MLabel>
#include <MSeekBar>
#include <MSceneManager>

#include "playerwidget.h"

/**
  * \mainpage Policy-aware Player
  *
  * Demonstrates how to use libresourceqt to create a policy-aware application.
  *
  * There are two classes:
  * \see PlayerPage creates the UI consisting of a control bar and a video widget
  * \see PlayerWidget contains all of policy-related customizations
  * to make the application behave well in concurrent world.
  *
  */

/**
  * PlayerPage class creates the UI of the application.  It contains a menu,
  * a control bar to control the playback, and PlayerWidget that does the actual playback.
  * For policy-related functionality please refer to PlayerWidget, this class merely
  * creates the mechanics of the UI for the demo application.
  *
  * \see PlayerWidget
  * \see MApplicationPage
  */

class PlayerPage : public MApplicationPage
{
    Q_OBJECT
public:
    PlayerPage();

private:
    void     createContent();
    void     makeControlBar(MLinearLayoutPolicy *controlBarPolicy);
    void     orientationChangeEvent(MOrientationChangeEvent *event);
    void     openFile(QString dir);
    void     setEnabled(MWidget *widget, bool enabled);
    MButton* makeButton(QString iconID, bool enabled = true);
    void     updateLabelsVisibility();

    MLinearLayoutPolicy *layoutPolicy, *controlBarPolicy;
    MButton             *btnLoadAudio, *btnLoadVideo, *btnPlay;
    MSeekBar            *seekbar;
    PlayerWidget        *playerWidget;
    MLabel              *lblTitle, *lblPosition;
    MAction             *menuAction;

    bool seekbarPressed;
    bool labelsVisible;

private slots:
    void openAudioFile();
    void openVideoFile();

    void togglePolicyAwareness();
    void setPlayingIcon();
    void setPausedIcon();
    void playOrPause();
    void onPositionChanged();
    void sliderPressed();
    void seekToNewPosition();
};
