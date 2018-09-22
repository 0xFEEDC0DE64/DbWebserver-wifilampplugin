#include "wifilampplugin.h"

#include "wifilampapplication.h"

WifiLampPlugin::WifiLampPlugin(QObject *parent) :
    WebPlugin(parent)
{

}

QString WifiLampPlugin::pluginName() const
{
    return QStringLiteral("wifilamp");
}

WebApplication *WifiLampPlugin::createApplication(const QJsonObject &config, WebServer &webServer) const
{
    return new WifiLampApplication(config, webServer);
}
