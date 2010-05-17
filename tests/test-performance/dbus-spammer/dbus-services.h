#ifndef _DBUS_THREAD_
#define _DBUS_THREAD_

#include <QList>
#include <QtDBus/QtDBus>
#include <QtCore/QObject>
#include <QtCore/QCoreApplication>
#include <dbus/dbus.h>

typedef QList<QThread*> Threads;
typedef QList<QObject*> Servers;

class Maintainer : public QObject
{
    Q_OBJECT

public:
    Maintainer(Threads* t);
    ~Maintainer();

protected:
    void timerEvent(QTimerEvent *e);

    int         timerID;
    Threads*    threads;
    bool        allOnline;
};

class DbusSpammer : public QThread
{
public:
    DbusSpammer(int id, QString service, int messageLen);
    ~DbusSpammer();

    void run();

    DBusConnection* myBus;
    DBusMessage*    myMessage;

    bool            firstTime;
    bool            exitThread;
    int             threadID;
    const char*     pszSpamData;
    QString         spamData;
    QString         serviceName;

protected:
    void timerEvent(QTimerEvent *e);

    int timerID;
};

class DbusServer: public QThread
{
    Q_OBJECT

public:
    DbusServer(int id);
    ~DbusServer();

    const QString& getServiceName() { return serviceName; }
    const int getServiceID() { return serviceID; }
    void run();

private:
    QDBusConnection* myBus;
    QString     serviceName;
    int         serviceID;

public slots:
    Q_SCRIPTABLE QString ping(const QString &arg);
};

#endif // _DBUS_THREAD_
