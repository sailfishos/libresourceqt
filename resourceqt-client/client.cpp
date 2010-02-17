#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>

#include "client.h"

Client::Client(QString appClass) : QObject()
{
	applicationClass 	= appClass;
	resourceSet			= NULL;

    standardInput = new QTextStream(stdin, QFile::ReadOnly);
	mainTimerID   = startTimer(0);
}

Client::~Client()
{
	killTimer(mainTimerID);

	if( resourceSet != NULL )
	{
		delete resourceSet;
		resourceSet = NULL;
	}

	if( standardInput != NULL )
	{
		delete standardInput;
		standardInput = NULL;
	}
}

uint32_t Client::parseResourceList(QString resourceListStr)
{
    struct
    {
    	uint32_t	resourceType;
    	const char*	resourceName;
    }
    resourceDef[] =
    {
        { RES_AUDIO_PLAYBACK ,  "AudioPlayback"  },
        { RES_VIDEO_PLAYBACK ,  "VideoPlayback"  },
        { RES_AUDIO_RECORDING,  "AudioRecording" },
        { RES_VIDEO_RECORDING,  "VideoRecording" },
        { RES_VIBRA          ,  "Vibra"          },
        { RES_LEDS           ,  "Leds"           },
        { RES_BACKLIGHT      ,  "BackLight"      },
        { RES_SYSTEM_BUTTON  ,  "SystemButton"   },
        { RES_LOCK_BUTTON    ,  "LockButton"     },
        { RES_SCALE_BUTTON   ,  "ScaleButton"    },
        { RES_SNAP_BUTTON    ,  "SnapButton"     },
        { RES_LENS_COVER     ,  "LensCover"      },
        {        0           ,       NULL        }
    };

    uint32_t  resourceList = 0;

    if( resourceListStr.isEmpty() || resourceListStr.isNull() )
    {
        return 0;
    }
    else
    {
    	QStringList resList = resourceListStr.split(",", QString::SkipEmptyParts);

    	for( int i = 0; i < resList.count(); i++ )
        {
    		int pos = 0;
            while( resourceDef[pos].resourceName != NULL )
            {
                if( resList[i] == resourceDef[pos].resourceName )
                    break;

                pos++;
            }

            if( !resourceDef[pos].resourceName )
            {
            	const char* res = qPrintable(resList[i]);
            	printf("Ignoring invalid resource name '%s'\n", res);
            }
            else
            {
            	resourceList |= resourceDef[pos].resourceType;
            }
        }
    }

    return resourceList;
}

void Client::showPrompt()
{
	printf("res-client> ");
	fflush(stdout);
}

void Client::updateSet(uint32_t list, uint32_t optional, bool remove)
{
	uint32_t resources[] =
	{
		RES_AUDIO_PLAYBACK, RES_VIDEO_PLAYBACK, RES_AUDIO_RECORDING, RES_VIDEO_RECORDING,
		RES_VIBRA, RES_LEDS, RES_BACKLIGHT, RES_SYSTEM_BUTTON, RES_LOCK_BUTTON,
		RES_SCALE_BUTTON, RES_SNAP_BUTTON, RES_LENS_COVER, 0
	};

	int pos = 0;
	while( resources[pos] )
	{
		if( list & resources[pos] )
		{
			Resource* resource = NULL;
			ResourceType res = getResourceType(resources[pos]);
			bool opt = (optional & resources[pos]) == resources[pos];

			if( remove )
			{
				if( !resourceSet->contains(res) )
				{
					continue;
				}

				if( optional )
				{
					resource = resourceSet->resource(res);
					resource->setOptional(false);
				}
				else
				{
					resourceSet->deleteResource(res);
				}
			}
			else
			{
				if( resourceSet->contains(res) )
				{
					resource = resourceSet->resource(res);
					if( resource->isOptional() != opt )
					{
						resource->setOptional(opt);
					}

					continue;
				}

				resource = allocateResource(res, opt);
				if( resource )
				{
					resourceSet->addResource(resource);
				}
			}
		}

		pos++;
	}
}

bool Client::initialize(uint32_t all, uint32_t optional, bool alwaysReply, bool autoRelease)
{
	resourceSet = new ResourceSet(applicationClass);
	if( resourceSet == NULL )
	{
		return false;
	}

	if( alwaysReply )
	{
	    qDebug("client: alwaysReply");
		resourceSet->setAlwaysReply();
	}

	if( autoRelease )
	{
	    qDebug("client: autoRelease");
		resourceSet->setAutoRelease();
	}

	updateSet(all, optional, false);

	if( !connect(resourceSet, SIGNAL(resourcesGranted(QList<ResourceType>)), this, SLOT(resourceAcquiredHandler(QList<ResourceType>))) )
	{
		return false;
	}

	if( !connect(resourceSet, SIGNAL(resourcesDenied()), this, SLOT(resourceDeniedHandler())) )
	{
		return false;
	}

	if( !connect(resourceSet, SIGNAL(lostResources()), this, SLOT(resourceLostHandler())) )
	{
		return false;
	}
    connect(resourceSet, SIGNAL(resourcesReleased()), this, SLOT(resourceReleasedHandler()));

	showPrompt();

	return true;
}

Resource* Client::allocateResource(ResourceType resource, bool optional)
{
	Resource* retValue = NULL;

	switch( resource )
	{
	case AudioPlaybackType:
		retValue = new AudioResource();
		break;
	case VideoPlaybackType:
		retValue = new VideoResource();
		break;
	case AudioRecorderType:
		retValue = new AudioRecorderResource();
		break;
	case VideoRecorderType:
		retValue = new VideoRecorderResource();
		break;
	case VibraType:
		retValue = new VibraResource();
		break;
	case LedsType:
		retValue = new LedsResource();
		break;
	case BacklightType:
		retValue = new BacklightResource();
		break;
	case SystemButtonType:
		retValue = new SystemButtonResource();
		break;
	case LockButtonType:
		retValue = new LockButtonResource();
		break;
	case ScaleButtonType:
		retValue = new ScaleButtonResource();
		break;
	case SnapButtonType:
		retValue = new SnapButtonResource();
		break;
	case LensCoverType:
		retValue = new LensCoverResource();
		break;
	case NumberOfTypes:
		return NULL;
	}

	if( retValue )
	{
		retValue->setOptional(optional);
	}
	else
	{
		printf("Unknown resource type - %d\n", resource);
	}

	return retValue;
}

ResourceType Client::getResourceType(uint32_t resource)
{
	switch( resource )
	{
	case RES_AUDIO_PLAYBACK:
		return AudioPlaybackType;
	case RES_VIDEO_PLAYBACK:
		return VideoPlaybackType;
	case RES_AUDIO_RECORDING:
		return AudioRecorderType;
	case RES_VIDEO_RECORDING:
		return VideoRecorderType;
	case RES_VIBRA:
		return VibraType;
	case RES_LEDS:
		return LedsType;
	case RES_BACKLIGHT:
		return BacklightType;
	case RES_SYSTEM_BUTTON:
		return SystemButtonType;
	case RES_LOCK_BUTTON:
		return LockButtonType;
	case RES_SCALE_BUTTON:
	    return ScaleButtonType;
	case RES_SNAP_BUTTON:
	    return SnapButtonType;
	case RES_LENS_COVER:
	    return LensCoverType;
	}

	return NumberOfTypes;
}

void Client::showResources(const QList<ResourceType> resList)
{
    const char* resTypes[] =
	{
		"AudioPlayback", "VideoPlayback", "AudioRecorder", "VideoRecorder", "Vibra",
		"Leds", "Backlight", "SystemButton", "LockButton", "ScaleButton", "SnapButton",
		"LensCover"
	};

	for( int i = 0; i < resList.count(); i++ )
	{
		printf("\t%s\n", resTypes[resList[i]]);
	}
}

void Client::showResources(const QList<Resource*> resList)
{
    const char* resTypes[] =
	{
		"AudioPlayback", "VideoPlayback", "AudioRecorder", "VideoRecorder", "Vibra",
		"Leds", "Backlight", "SystemButton", "LockButton", "ScaleButton", "SnapButton",
		"LensCover"
	};

	for( int i = 0; i < resList.count(); i++ )
	{
		Resource* r = resList[i];
		printf("\t%s%s%s\n", resTypes[r->type()],
				r->isOptional() ? " (optional)" : "",
				r->isGranted() ? " (granted)" : "");
	}
}

void Client::resourceAcquiredHandler(const QList<ResourceType>& /*grantedResList*/)
{
	QList<Resource*> list = resourceSet->resources();
	if( !list.count() )
	{
		printf("\nGranted resource set is empty. Possible bug?\n");
	}
	else
	{
		printf("\nManager grants access to these resources:\n");
		printf("Resource set:\n");
		showResources(list);
	}
	showPrompt();
}

void Client::resourceDeniedHandler()
{
	printf("\nManager denies access to resources!\n");
	showPrompt();
}

void Client::resourceLostHandler()
{
	printf("\nLost resources from manager!\n");
	showPrompt();
}

void Client::resourceReleasedHandler()
{
	printf("\nAll resources released\n");
	showPrompt();
}

void Client::timerEvent(QTimerEvent*)
{
	bool quitFlag = false;

	fd_set stdinfd;
	FD_ZERO(&stdinfd);
	FD_SET(0, &stdinfd);
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	int ready = select(1, &stdinfd, NULL, NULL, &tv);
	if( ready > 0 )
	{
		QString line = standardInput->readLine();
		if( !line.isNull() )
		{
			QStringList params = line.split(" ");
			if( params[0] == "quit" )
			{
				quitFlag = true;
				QMetaObject::invokeMethod(QCoreApplication::instance(), "quit");
			}
			else if( params[0] == "help" )
			{
			    printf("Available commands:\n");
			    printf("\t  help   \tprint this help message\n");
			    printf("\t  quit   \texit application\n");
			    printf("\t  acquire\tacquire required resources\n");
			    printf("\t  release\trelease resources\n");
			    printf("\t  update\tupdate modified resource set after add or remove command\n");
			    printf("\t  add reslist [-o]\tadd reosurce list, if -o provided, set as optional\n");
			    printf("\t  remove reslist [-o]\tremove reosurce list, if -o provided, removed only optional flag\n");
			    printf("\t  audio pid <pid>|group <audio group>|tag <name:value>\tset audio properties");
			    printf("\t  show  \tshow resources\n");
			}
			else if( params[0] == "show" )
			{
				if( !resourceSet )
				{
					printf("Show failed!\n");
				}
				else
				{
					QList<Resource*> list = resourceSet->resources();
					if( !list.count() )
					{
						printf("Resource set is empty, use add command to add some ...\n");
					}
					else
					{
						printf("Resource set:\n");
						showResources(list);
					}
				}
			}
			else if( params[0] == "acquire" )
			{
				if( !resourceSet || !resourceSet->acquire() )
				{
					printf("Acquire failed!\n");
				}
			}
			else if( params[0] == "release" )
			{
				if( !resourceSet || !resourceSet->release() )
				{
					printf("Release failed!\n");
				}
			}
			else if( params[0] == "add" )
			{
				if( !resourceSet )
				{
					printf("Update failed!\n");
				}
				else if( params.count() == 1 || params[1].isEmpty() || params[1].isNull() )
				{
					printf("List of desired resources is missing. See help ...\n");
				}
				else
				{
					uint32_t temp = Client::parseResourceList(params[1]);
					if( params.count() > 2 && params[2] == "-o" )
					{
						updateSet(temp, temp, false);
					}
					else
					{
						updateSet(temp, 0, false);
					}
				}
			}
			else if( params[0] == "remove" )
			{
				if( !resourceSet || params.count() == 1 )
				{
					printf("Update failed!\n");
				}
				else if( params.count() == 1 || params[1].isEmpty() || params[1].isNull() )
				{
					printf("List of desired resources is missing. See help ...\n");
				}
				else
				{
					uint32_t temp = Client::parseResourceList(params[1]);
					if( params.count() > 2 && params[2] == "-o" )
					{
						updateSet(temp, temp, true);
					}
					else
					{
						updateSet(temp, 0, true);
					}
				}
			}
			else if( params[0] == "update" )
			{
				if( !resourceSet || !resourceSet->update() )
				{
					printf("Update failed!\n");
				}
			}
			else if( params[0] == "audio" )
			{
				if( params.count() < 3 )
				{
					printf("Not enough parameters! See help!\n");
				}
				else
				{
				    Resource *resource = resourceSet->resource(AudioPlaybackType);
					AudioResource *audioResource = static_cast<AudioResource*>(resource);
                    qDebug("resource = %p audioResource = %p", resource, audioResource);
					if( audioResource == NULL )
					{
						printf("No AudioResource available in set!\n");
					}
					else
					{
					    if( params[1] == "group" )
					    {
					        audioResource->setAudioGroup(params[2]);
				        }
				        else if( params[1] == "pid" )
				        {
				            bool ok;
    						quint32 pid = (quint32)params[2].toInt(&ok, 10);
						    if( ok && pid != 0 )
    						{
	    						audioResource->setProcessID(pid);
	    					}
	    					else
	    					{
	    						printf("Bad pid parameter!\n");
	    					}
    					}
    					else if( params[1] == "tag" ) {
    					    audioResource->setStreamTag(params[2]);
						}
					    else {
					        printf("Unknown audio command!\n");
					    }
					}
				}
			}
			else if( !params[0].isEmpty() )
			{
				QByteArray ba = line.toLatin1();
				const char *c_line = ba.data();
				printf("unknown command '%s'\n", c_line);
			}

			if( !quitFlag )
			{
				showPrompt();
			}
		}
	}
}
