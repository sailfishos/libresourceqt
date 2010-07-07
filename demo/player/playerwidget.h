#pragma once

#include <MVideoWidget>
#include <policy/resource-set.h>

/**
  * PlayerWidget subclasses MVideoWidget from libmeegotouch to add policy awareness to this
  * widget and also some convenience methods / overrides for our demo.
  *
  * MVideoWidget is the control for video playback, but apparently also outputs audio as well.
  * It has the basic play(), pause(), seek() functions and contains the state of the playback.
  *
  * We override play() and pause() so that if demo is running in the policy-aware mode (the default),
  * an audio resource is acquired for each playback operation.
  *
  * \see MVideoWidget
  */
class PlayerWidget : public MVideoWidget {
  Q_OBJECT

public:
  PlayerWidget(QGraphicsItem *parent = 0);

  enum {VIDEO, AUDIO} filetype;

  void play();
  void beginPlayback();
  void pause(bool releaseResources = true);
  void acquire();
  void release();

  bool policyAware();
  void setPolicyAware(bool aware);

  quint64 position();
  void setPosition(quint64);
  void seek(quint64);

private:

  ResourcePolicy::ResourceSet *resourceSet;
  ResourcePolicy::AudioResource *audioResource;

  struct data {
    quint64 pos;
    bool    policyAware;

    data() : pos(0), policyAware(true)  {}
  } d;

  void timerEvent(QTimerEvent *event);

private slots:
  void resourceAcquiredHandler(const QList<ResourcePolicy::ResourceType>& /*grantedOptionalResList*/);
  void resourceReleasedHandler();
  void resourceLostHandler();

signals:
  void playerPositionChanged();
  void playing();
  void paused();

};
