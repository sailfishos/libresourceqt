#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <QObject>
#include <QtCore/QTextStream>

#include <stdint.h>
#include <policy/resource-set.h>
#include <sys/resource.h>

#include "commandlineparser.h"
#include "time-stat.h"

class Client : public QObject
{
	Q_OBJECT

public:
	Client();
	~Client();

	bool initialize(const CommandLineParser &commandParser);
	static uint32_t parseResourceList(QString resourceListStr);

private slots:
	void resourceAcquiredHandler(const QList<ResourcePolicy::ResourceType>& grantedResList);
	void resourceDeniedHandler();
	void resourceLostHandler();
	void resourceReleasedHandler();
	void resourcesBecameAvailableHandler(const QList<ResourcePolicy::ResourceType> &availableResources);

protected:
    void timerEvent(QTimerEvent *e);

private:
    QTextStream standardInput;
    int mainTimerID;

	QString	applicationClass;

	ResourcePolicy::ResourceSet *resourceSet;

	ResourcePolicy::Resource* allocateResource(ResourcePolicy::ResourceType resource, bool optional);
	ResourcePolicy::ResourceType getResourceType(uint32_t resource);

	void showPrompt();
	void showResources(const QList<ResourcePolicy::ResourceType> resList);
	void showResources(const QList<ResourcePolicy::Resource*> resList);
	void updateSet(uint32_t list, uint32_t optional, bool remove);
};

#endif

