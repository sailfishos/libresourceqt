#include <string.h>
#include <stdlib.h>
#include <gst/interfaces/xoverlay.h>

#include "streamer.h"

static char location_file[256] = "./default.mp3";

static void pad_added(GstElement *element __attribute__ ((unused)),
                      GstPad *pad, gpointer data)
{
  GstCaps *caps;
  GstStructure *str;

  GstPad *sink_pad = NULL;
  streamer_t *streamer = (streamer_t *)data;

  caps = gst_pad_get_caps(pad);
  str = gst_caps_get_structure (caps, 0);

  if (!str || gst_pad_is_linked(pad))
    return;

  if (g_strrstr(gst_structure_get_name(str), "audio")) {
    sink_pad = gst_element_get_compatible_pad(streamer->audio_queue, pad, NULL);
  }

  if (sink_pad) {
    gst_pad_link(pad, sink_pad);
  }
}

void Streamer::busSyncCallback(GstBus *bus, GstMessage *message)
{
  Q_UNUSED(bus);

  gst_message_unref(message);
}

void Streamer::busCallback(GstBus *bus, GstMessage *message)
{
  GError *err;
  gchar *debug;

  Q_UNUSED(bus);

  switch (GST_MESSAGE_TYPE (message)) {
  case GST_MESSAGE_ERROR:
    gst_message_parse_error(message, &err, &debug);
    g_printerr("%s\n", err->message);
    emit error(QString(err->message));
    g_error_free(err);
    g_free(debug);

    break;
  case GST_MESSAGE_EOS:
    /* end-of-stream */
    emit eos();

    break;
  default:
    /* unhandled message */
    break;
  }
}

void Streamer::attachBusCallbacks(GstElement *pipeline)
{
  GstBus *bus;

  bus = gst_pipeline_get_bus(GST_PIPELINE (pipeline));

  gst_bus_set_sync_handler(bus, (GstBusSyncHandler) bus_sync_callback, this);
  gst_bus_add_watch(bus, bus_callback, this);

  gst_object_unref(bus);
}

QBool Streamer::initPlayback(streamer_t *streamer)
{
/* create GStreamer elements */
  streamer->file_source = gst_element_factory_make("filesrc",     "file source");
  streamer->decoder     = gst_element_factory_make("decodebin",   "universal decoder");
  streamer->audio_sink  = gst_element_factory_make("pulsesink",   "pulseaudio sink");
  streamer->audio_queue = gst_element_factory_make("queue",       "audio queue");

  if (!streamer->file_source || !streamer->decoder ||
      !streamer->audio_sink  ||
      !streamer->audio_queue) {
    emit error(QString("One of pipeline elements can't be created"));
    return QBool(false);
  }

  /* Set options for GStreamer elements */
  g_object_set(G_OBJECT(streamer->file_source), "location", location_file, NULL);

  /* add elements to pipeline before linking them */
  gst_bin_add_many(GST_BIN(streamer->pipeline),
                   streamer->file_source, streamer->decoder,
                   streamer->audio_queue, streamer->audio_sink,
                   NULL);

  /* Link elements */
  if (!gst_element_link(streamer->file_source, streamer->decoder)) {
    emit error(QString("Failed to link file source element"));
    return QBool(false);
  }

  if (!gst_element_link(streamer->audio_queue, streamer->audio_sink)) {
    emit error(QString("Failed to link audio sink element"));
    return QBool(false);
  }

  g_signal_connect(G_OBJECT(streamer->decoder), "pad-added",
                   G_CALLBACK(pad_added), streamer);

  return QBool(true);
}

void Streamer::play()
{
  mutex.lock();

  if ( streamer.pipeline && GST_STATE(streamer.pipeline) == GST_STATE_PAUSED ) {
    gst_element_set_state(streamer.pipeline, GST_STATE_PLAYING);
    mutex.unlock();
    return;
  }

  streamer.pipeline = gst_pipeline_new("Playback pipeline");

  if (!initPlayback(&streamer)) {
    gst_object_unref(GST_OBJECT(streamer.pipeline));
    streamer.pipeline = NULL;
    mutex.unlock();    
    return;
  }

  /* add callbacks for pipeline's message bus */
  attachBusCallbacks(streamer.pipeline);

  gst_element_set_state(streamer.pipeline, GST_STATE_PLAYING);

  mutex.unlock();
}


void Streamer::pause(void)
{
  GstElement *pipeline;

  mutex.lock();
  pipeline = streamer.pipeline;

  if (pipeline) {
   gst_element_set_state(pipeline, GST_STATE_PAUSED);
  }
  mutex.unlock();
}



void Streamer::stop(void)
{
  GstElement *pipeline;

  mutex.lock();
  pipeline = streamer.pipeline;

  if (pipeline) {
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));
    streamer.pipeline = NULL;
  }
  mutex.unlock();
}

void Streamer::setLocation(const QString location)
{
  mutex.lock();
  strncpy(location_file, (const char*)location.toAscii().data(), 256);
  location_file[255] = 0;
  /* Set options for GStreamer elements */
  g_object_set(G_OBJECT(streamer.file_source), "location", location_file, NULL);

  printf("streamer> location: %s\n", location_file);

  mutex.unlock();
}

void Streamer::run()
{
  mutex.lock();
  g_type_init();
  gst_init(NULL, NULL);
  memset(&streamer, 0, sizeof(streamer_t));
  mutex.unlock();

  while (!quit) {
    mutex.lock();
    cond.wait(&mutex);
    mutex.unlock();
  }
}

Streamer::Streamer(QObject *parent)
    : QThread(parent),
    quit(false)
{
}

Streamer::~Streamer()
{
  mutex.lock();
  quit = true;
  cond.wakeOne();
  mutex.unlock();

  wait();
}

void Streamer::setPosition(quint64 pos) {
gst_element_seek (streamer.pipeline, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
  GST_SEEK_TYPE_SET, pos*1000*1000,
  GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);

}

Streamer::State Streamer::state() {
  if (streamer.pipeline)
    return GST_STATE(streamer.pipeline) == GST_STATE_PLAYING ? Streamer::PlayingState : Streamer::PausedState;
  else
    return Streamer::StoppedState;
}

quint64 Streamer::duration() {
  if (streamer.pipeline) {

    GstFormat fmt = GST_FORMAT_TIME;
    gint64 pos, len;

    gst_element_query_position (streamer.pipeline, &fmt, &pos);
    gst_element_query_duration (streamer.pipeline, &fmt, &len);

    return (quint64)(len / 1000.0 / 1000.0); // ms
  }
  else {
    return 0;
  }
}
