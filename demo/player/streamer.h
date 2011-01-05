#pragma once

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include <gst/gst.h>

typedef struct {
    GstElement *pipeline;

    GstElement *file_source;
    GstElement *decoder;
    GstElement *audio_sink;
    GstElement *audio_queue;
    GstElement *video_sink;
    GstElement *video_queue;

    gulong video_windowid;
} streamer_t;

class Streamer : public QThread
{
    Q_OBJECT

public:
    Streamer(QObject *parent);
    ~Streamer();

    void play(void);
    void stop(void);
    void setLocation(const QString location);
    void setWindowid(gulong id);
    void setPosition(quint64 pos);

    enum State {
      PlayingState, PausedState, StoppedState
    };

    State state();
    quint64 duration();

signals:
    void error(const QString message);
    void eos(void);

protected:
    void run(void);

private:
    void busCallback(GstBus *bus, GstMessage *message);
    void busSyncCallback(GstBus *bus, GstMessage *message);

    void attachBusCallbacks(GstElement *pipeline);
    QBool initPlayback(streamer_t *streamer);

    static gboolean bus_callback(GstBus *bus, GstMessage *message, gpointer data)
    {
	reinterpret_cast<Streamer*>(data)->busCallback(bus, message);
	return TRUE;
    }

    static GstBusSyncReply bus_sync_callback(GstBus *bus, GstMessage *message, gpointer data)
    {
	if (GST_MESSAGE_TYPE (message) != GST_MESSAGE_ELEMENT ||
	    !gst_structure_has_name(message->structure, "prepare-xwindow-id"))
	    return GST_BUS_PASS;

	reinterpret_cast<Streamer*>(data)->busSyncCallback(bus, message);
	return GST_BUS_DROP;
    }

    streamer_t streamer;

    QMutex mutex;
    QWaitCondition cond;
    bool quit;
};
