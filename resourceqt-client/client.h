#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <QObject>
#include <QtCore/QTextStream>

#include <policy/resource-set.h>

#define RES_AUDIO_PLAYBACK	(1<<0)
#define RES_VIDEO_PLAYBACK	(1<<1)
#define RES_AUDIO_RECORDING	(1<<2)
#define RES_VIDEO_RECORDING	(1<<3)
#define RES_VIBRA			(1<<4)
#define RES_LEDS			(1<<5)
#define RES_BACKLIGHT		(1<<6)
#define RES_SYSTEM_BUTTON	(1<<7)
#define RES_LOCK_BUTTON		(1<<8)
#define RES_SCALE_BUTTON	(1<<9)
#define RES_SNAP_BUTTON		(1<<10)
#define RES_LENS_COVER		(1<<11)

class Client : public QObject
{
	Q_OBJECT

public:
	Client(QString appClass);
	~Client();

	bool initialize(uint32_t all, uint32_t optional, bool alwaysReply, bool autoRelease);
	static uint32_t parseResourceList(QString resourceListStr);

private slots:
	void resourceAcquiredHandler(const QList<ResourcePolicy::ResourceType>& grantedResList);
	void resourceDeniedHandler();
	void resourceLostHandler();
	void resourceReleasedHandler();

protected:
    void timerEvent(QTimerEvent *e);

private:
    QTextStream*	standardInput;
    int				mainTimerID;

	uint32_t		resourcesAll;
	uint32_t		resourcesOptional;
	QString			applicationClass;

	ResourcePolicy::ResourceSet* 	resourceSet;

	ResourcePolicy::Resource* allocateResource(ResourcePolicy::ResourceType resource, bool optional);
	ResourcePolicy::ResourceType getResourceType(uint32_t resource);

	void showPrompt();
	void showResources(const QList<ResourcePolicy::ResourceType> resList);
	void showResources(const QList<ResourcePolicy::Resource*> resList);
	void updateSet(uint32_t list, uint32_t optional, bool remove);
};

#endif
