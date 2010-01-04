#include "resource-library.h"
#include "resource.h"
#include <libplayback/playback.h>

#ifndef LIBPLAYBACKWRAPPER_H
#define LIBPLAYBACKWRAPPER_H

class LibPlaybackWrapper: public QObject, public ResourceLibrary
{
   Q_OBJECT
   Q_DISABLE_COPY( LibPlaybackWrapper );
private:
   DBusConnection *dbusConnection;
   pb_playback_t *libPlaybackHandle;
   Resource *resource;
public:
   LibPlaybackWrapper(Resource *resource);
   ~LibPlaybackWrapper();
   bool initialize();
   bool connectToServer();
};

void libPlaybackStateHandler(pb_playback_t *libPlaybackHandle, enum pb_state_e requestedState,
			     pb_req_t* playbackRequest, void *data);

inline enum pb_class_e resourceClassToLibPlaybackClass(enum ResourceClass resourceClass);
inline quint16 resourceFlagToLibPlaybackFlags(quint16 resourceFlag);
#endif
