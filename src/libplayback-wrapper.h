#ifndef LIBPLAYBACKWRAPPER_H
#define LIBPLAYBACKWRAPPER_H

#include "resource-library.h"
#include "resource.h"
#include <libplayback/playback.h>

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

   void stateChanged(enum pb_state_e newState);
   void hintReceived(const int allowedStates[]);
};

#endif
