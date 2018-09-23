#pragma once

#include "webapplication.h"

#include <QAbstractSocket>
#include <QHostAddress>

class QJsonObject;
class QTcpServer;

class WebServer;
class WifiLampClient;

class WifiLampApplication : public WebApplication
{
    Q_OBJECT
    static const QString SERVER_NAME;

public:
    WifiLampApplication(const QJsonObject &config, WebServer &webServer);

    void start() Q_DECL_OVERRIDE;

    void handleRequest(HttpClientConnection *connection, const HttpRequest &request) Q_DECL_OVERRIDE;

    const QSet<WifiLampClient*> &clients() const;

private Q_SLOTS:
    void acceptError(QAbstractSocket::SocketError socketError);
    void newConnection();

private:
    void handleRoot(HttpClientConnection *connection, const HttpRequest &request);
    void redirectRoot(HttpClientConnection *connection, const HttpRequest &request);
    static QString clientId(const WifiLampClient *client, bool forceIp = false);

    WebServer &m_webServer;

    QHostAddress m_hostAddress;
    quint16 m_port;
    QTcpServer *m_tcpServer;

    QSet<WifiLampClient*> m_clients;
};
