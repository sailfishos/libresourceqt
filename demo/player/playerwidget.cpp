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

#include "playerwidget.h"

using namespace ResourcePolicy;

/**
  * Creates a resource set in the "player" application class, this application class
  * best suites our application description. The application class is used to determine the priority
  * order of the application.
  * We create one AudioResource object and add it to the set.  For that audio resource we
  * have to set the pid of the audio renderer, which in our case is the same as the pid of the application,
  * and stream tag, which we simply set to an all-allowing "*".
  *
  * Also, we connect to three signals:
  * - resourcesGranted, which is triggered whenever the resource we asked for is granted to us;
  * - lostResources, which is triggered whenever another application has been granted with our resource
  * and we are no longer able to use it;
  * - resourcesReleased, which is triggered whenever a call to release() is done and the resource
  * has been successfully released.
  *
  * At last, we add a timer which fires every 100 milliseconds in order to update the current playback position.
  * This is done because MVideoWidget currently only updates said position for video files, and not for audio.
  * It might not be as precise, but will serve our needs for the demo.
  *
  * \see PlayerWidget::resourceAcquiredHandler
  * \see PlayerWidget::resourceLostHandler
  * \see PlayerWidget::resourceReleasedHandler
  */
PlayerWidget::PlayerWidget(Streamer *streamer)
  : QObject() {

  this->streamer = streamer;
  this->streamer->start();

  connect(this->streamer, SIGNAL(eos()), this, SLOT(eos()));
  connect(this->streamer, SIGNAL(error(const QString)), this, SLOT(error(const QString)));

  qDebug("PlayerWidget::PlayerWidget");
  resourceSet = new ResourceSet("player", this);
//  resourceSet->setAlwaysReply();
//  resourceSet->setAutoRelease();

  audioResource = new ResourcePolicy::AudioResource("player");
  audioResource->setProcessID(QCoreApplication::applicationPid());
  audioResource->setStreamTag("media.name", "*");
  resourceSet->addResourceObject(audioResource);

  connect(resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType>&)),
          this,        SLOT(resourceAcquiredHandler(const QList<ResourcePolicy::ResourceType>&)));
  connect(resourceSet, SIGNAL(lostResources()),     this, SLOT(resourceLostHandler()));
  connect(resourceSet, SIGNAL(resourcesReleased()), this, SLOT(resourceReleasedHandler()));

  // playback timer
  startTimer(100);
}

void PlayerWidget::error(const QString message)
{
    qCritical() << QString("Streamer error: %1").arg(message);

    pause();
}

void PlayerWidget::eos(void)
{
    qDebug() << QString("end of stream");

    pause();
}

/**
  * Calls acquire() on the resource set.  Shortly after we should receive a notification
  * when resources are granted, which means we can begin using audio output.
  *
  * \see PlayerWidget::resourceAcquiredHandler
  *
  */
void PlayerWidget::acquire() {
  if (filetype == VIDEO) {
    resourceSet->addResource(ResourcePolicy::VideoPlaybackType);
  } else {
    resourceSet->deleteResource(ResourcePolicy::VideoPlaybackType);
  }

  resourceSet->acquire();
}

/**
  * Calls release() on the resource set.  Called whenever we are not using the resource.
  * We can acquire the same resource set again if required.  We release resource every time
  * a pause button is pressed or audio playback is finished, so that other applications can
  * make use of it.
  * Call to release() yields resourceReleased() signal shortly after to inform us that resource
  * has indeed been released.
  *
  * \see PlayerWidget::resourceReleasedHandler
  *
  */
void PlayerWidget::release() {
  resourceSet->release();
}

/**
  * Overrides parent play() method to add wrapping with resource policy handling.
  * If we are in resource policy aware mode, asks to acquire the resource,
  * otherwise begins playback immediately.
  *
  * \see MVideoWidget::play().
  */
void PlayerWidget::play() {
  qDebug("PlayerWidget::play()");
  if (policyAware())
    acquire();
  else
    beginPlayback();
}

/**
  * Calls parent play() method to begin playback and emits the playing() signal.
  * This function is used in place of play(), which only prepares the playback
  * prior to actually starting it.
  *
  */
void PlayerWidget::beginPlayback() {
  streamer->play();
  emit playing();
}


/**
  * Overrides parent pause() method.  Releases the audio resource on pause so
  * that another application can start using it immediately.  Emits paused() signal.
  * An optional parameter releaseResources is added to opt out of releasing the
  * resource in PlayerWidget::resourceLostHandler() handler.
  *
  * \see MVideoWidget::pause().
  */
void PlayerWidget::pause(bool releaseResources) {
  streamer->stop();
  if (releaseResources && policyAware())  release();
  emit paused();
}

/**
  * The most interesting kind of event in resource policy.  This event
  * arrives to notify us that the resources we asked for are available for us to use.
  * Note that not all resources of the resource set might be granted, but we have
  * only one resource in total (audio).  If the granted resources list is not empty,
  * we begin the playback.
  *
  * \see signal ResourcePolicy::ResourceSet::resourcesGranted().
  */
void PlayerWidget::resourceAcquiredHandler(const QList<ResourcePolicy::ResourceType>& /*grantedOptionalResList*/) {

  qDebug("PlayerWidget::resourceAcquiredHandler()");
  beginPlayback();
}

/**
  * This notification arrives to acknowledge that we no longer own the resources from
  * our resource set.  The handler is a no-op in our case.
  *
  * \see signal ResourcePolicy::ResourceSet::resourcesReleased().
  */
void PlayerWidget::resourceReleasedHandler() {
  qDebug("PlayerWidget::resourceReleasedHandler()");
}

/**
  * Handles the event of a resource being taken by another application.
  * Our application is forced to stop using audio resource by muting the sound output.
  * We will pause the audio, but will not release the resource yet,
  * so that we receive the acquire notification as soon as the audio resource
  * is available again.
  *
  * \see signal ResourcePolicy::ResourceSet::lostResources().
  */
void PlayerWidget::resourceLostHandler() {
  qDebug("PlayerWidget::resourceLostHandler()");
  if (state() == Streamer::PlayingState) {
    pause(false);
  }
}

/**
  * Timer event is used to update current playback position, since MVideoWidget updates it
  * only for video files and not audio files.  Our own instance variable, d.pos, is used.
  * It also handles the Stopped state, in which case paused() signal will be issued.
  * Every time we emit playerPositionChanged() for the subscribed widget to do its own
  * processing as well.
  *
  */
void PlayerWidget::timerEvent(QTimerEvent */*event*/) {
  // MVideoWidget doesn't update position on audio files, so we'll keep our own count
  // /* position = videoWidget->position(); */

  qDebug() << "PlayerWidget::timerEvent state=" << (int)state();

  if (state() == Streamer::PlayingState) {
    d.pos += 100;
  }

  if (state() == Streamer::StoppedState && prevState != state()) {
    d.pos = 0;
    pause();
    prevState = state();
  }

  emit playerPositionChanged();
}

/**
  * Getter for policyAware property, which can be set to true to make the demo behave
  * like policy-aware and to false for otherwise.
  */
bool PlayerWidget::policyAware() {
  return d.policyAware;
}

/**
  * Setter for policyAware property, which can be set to true to make the demo behave
  * like policy-aware and to false for otherwise.
  */
void PlayerWidget::setPolicyAware(bool aware) {
  d.policyAware = aware;

  if (policyAware()) {
    if (state() == Streamer::PlayingState)  acquire();
  } else {
    release();
  }
}

/**
  * Getter for position property, our own current playback position counter.  Note that
  * this overrides parent method, but we could call it with full qualification if needed.
  */
quint64 PlayerWidget::position() {
  return d.pos;
}

/**
  * Setter for position property, our own current playback position counter.  Note that
  * this overrides parent method, but we could call it with full qualification if needed.
  */
void PlayerWidget::setPosition(quint64 pos) {
  d.pos = pos;
}

/**
  * Overrides parent seek() method, which prior to seek() also updates our own playback position
  * counter.
  *
  * \see PlayerWidget::position()
  */
void PlayerWidget::seek(quint64 pos) {
  setPosition(pos);
  streamer->setPosition(pos);
}


void PlayerWidget::open(const QString& filename) {
  streamer->setLocation(filename);
}

Streamer::State PlayerWidget::state() {
  return streamer->state();
}

quint64 PlayerWidget::length() {
  return streamer->duration();
}

