#include "override.h"

#define MANAGER_PATH      "/org/maemo/Playback/Manager"
#define MANAGER_SERVICE   "org.maemo.Playback.Manager"
#define MANAGER_INTERFACE "org.maemo.Playback.Manager"

using namespace ResourcePolicy;

Override * ResourcePolicy::createMute(QObject *parent)
{
    return new Override("Mute", parent);
}

Override * ResourcePolicy::createPrivacyOverride(QObject *parent)
{
    return new Override("PrivacyOverride", parent);
}

Override * ResourcePolicy::createBluetoothOVerride(QObject *parent)
{
    return new Override("BluetoothOverride", parent);
}

Override::Override(QString overrideType, QObject *parent):
    QObject(parent), dBusConnection(QDBusConnection::sessionBus()), type(overrideType)
{
    dBusConnection.connect(MANAGER_SERVICE, MANAGER_PATH, MANAGER_INTERFACE,
                           type, this, SLOT(handleChange(bool)));
}

Override::~Override()
{
}

void Override::fetchState()
{
    QDBusMessage msg;
    QString method = "Get" + type;

    msg = QDBusMessage::createMethodCall(MANAGER_SERVICE, MANAGER_PATH, 
                                         MANAGER_INTERFACE, method);
    dBusConnection.send(msg);
}

void Override::request(bool newState)
{
    QDBusMessage msg;
    QString method = "Request" + type;

    msg = QDBusMessage::createMethodCall(MANAGER_SERVICE, MANAGER_PATH, 
                                         MANAGER_INTERFACE, method);

    QVariantList args;
    args.append(newState);
    msg.setArguments(args);
    dBusConnection.send(msg);
}

void Override::handleChange(bool newState)
{
    emit changed(newState);
}

