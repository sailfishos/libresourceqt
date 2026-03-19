#pragma once

#include <QtDBus>
#include <QDBusConnection>
#include <QDBusArgument>

namespace ResourcePolicy
{
    class Override : public QObject
    {
        Q_OBJECT
    public:
        Override(QString overrideType, QObject *parent=NULL);
        virtual ~Override();
        void fetchState();

    signals:
        void changed(bool newState);

    private slots:
        void handleChange(bool newState);
        void request(bool newState);

    private:
        QDBusConnection dBusConnection;
        QString type;
    };

    Override * createMute(QObject *parent = nullptr);
    Override * createPrivacyOverride(QObject *parent = nullptr);
    Override * createBluetoothOVerride(QObject *parent = nullptr);
}

