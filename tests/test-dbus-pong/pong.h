#ifndef _PONG_H_
#define _PONG_H_

#include <QtCore/QObject>

class Pong: public QObject
{
    Q_OBJECT
public slots:
    Q_SCRIPTABLE QString ping(const QString &arg);
    Q_SCRIPTABLE unsigned int quit();
    Q_SCRIPTABLE unsigned int timeout();
};

#endif	// _PONG_H_
