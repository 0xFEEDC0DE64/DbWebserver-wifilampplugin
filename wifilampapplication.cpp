#include "wifilampapplication.h"

#include <QJsonObject>
#include <QJsonValue>
#include <QTcpServer>
#include <QStringBuilder>

#include "utils/netutils.h"

#include "webserver.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "httpclientconnection.h"
#include "httpnotfoundexception.h"

#include "wifilampclient.h"

const QString WifiLampApplication::SERVER_NAME(QStringLiteral("WifiLamp Server 1.0"));

WifiLampApplication::WifiLampApplication(const QJsonObject &config, WebServer &webServer) :
    WebApplication(&webServer), m_webServer(webServer)
{
    if(!config.contains(QStringLiteral("controlHostAddress")))
        throw std::runtime_error("WifiLamp application does not contain controlHostAddress");

    const auto hostAddressVal = config.value(QStringLiteral("controlHostAddress"));
    if(!hostAddressVal.isString())
        throw std::runtime_error("WifiLamp application controlHostAddress is not a string");

    m_hostAddress = parseHostAddress(hostAddressVal.toString());

    if(!config.contains(QStringLiteral("controlPort")))
        throw std::runtime_error("WifiLamp application does not contain controlPort");

    const auto portVal = config.value(QStringLiteral("controlPort"));
    if(!portVal.isDouble())
        throw std::runtime_error("WifiLamp application controlPort is not a number");

    m_port = portVal.toInt();

    m_tcpServer = new QTcpServer(this);
}

void WifiLampApplication::start()
{
    if(!m_tcpServer->listen(m_hostAddress, m_port))
        throw std::runtime_error(QString("Could not start listening on %0:%1 because %2")
                                 .arg(m_hostAddress.toString()).arg(m_port).arg(m_tcpServer->errorString()).toStdString());

    connect(m_tcpServer, &QTcpServer::acceptError, this, &WifiLampApplication::acceptError);
    connect(m_tcpServer, &QTcpServer::newConnection, this, &WifiLampApplication::newConnection);
}

void WifiLampApplication::handleRequest(HttpClientConnection *connection, const HttpRequest &request)
{
    if(!request.path.startsWith('/'))
    {
        HttpResponse response;
        response.protocol = request.protocol;
        response.statusCode = HttpResponse::StatusCode::BadRequest;
        response.headers.insert(HttpResponse::HEADER_SERVER, SERVER_NAME);
        response.headers.insert(HttpResponse::HEADER_CONTENTTYPE, QStringLiteral("text/html"));

        connection->sendResponse(response, tr("Path does not start with /"));
        return;
    }

    if(request.path == QStringLiteral("/"))
    {
        handleRoot(connection, request);
    }
    else if(request.path == QStringLiteral("/refresh"))
    {
        for(auto client : m_clients)
            client->requestStatus();
        redirectRoot(connection, request);
    }
    else if(request.path.startsWith("/devices/"))
    {
        auto parts = request.path.split('/');
        if(parts.count() != 4)
            throw HttpNotFoundException(request);

        WifiLampClient *client;

        {
            auto iter = std::find_if(m_clients.constBegin(), m_clients.constEnd(),
                                     [&parts](auto client){ return clientId(client) == parts.at(2); });

            if(iter == m_clients.constEnd())
                throw HttpNotFoundException(request);

            client = *iter;
        }

        static const QHash<QString, std::function<void(WifiLampClient*)> > actions {
            { QStringLiteral("toggle"),  [](auto client){ client->toggle();        } },
            { QStringLiteral("on"),      [](auto client){ client->on();            } },
            { QStringLiteral("off"),     [](auto client){ client->off();           } },
            { QStringLiteral("refresh"), [](auto client){ client->requestStatus(); } },
            { QStringLiteral("reboot"),  [](auto client){ client->reboot();        } },
            { QStringLiteral("delete"),  [](auto client){ client->deleteLater();   } }
        };

        {
            auto iter = actions.find(parts.at(3));
            if(iter == actions.constEnd())
                throw HttpNotFoundException(request);
            else
            {
                (*iter)(client);
                redirectRoot(connection, request);
            }
        }
    }
    else
        throw HttpNotFoundException(request);
}

const QSet<WifiLampClient *> &WifiLampApplication::clients() const
{
    return m_clients;
}

void WifiLampApplication::acceptError(QAbstractSocket::SocketError socketError)
{
    qCritical() << socketError;
}

void WifiLampApplication::newConnection()
{
    auto socket = m_tcpServer->nextPendingConnection();
    if(!socket)
    {
        qWarning() << "null socket received";
        return;
    }

    auto client = new WifiLampClient(*socket, *this);
    m_clients.insert(client);
    connect(client, &QObject::destroyed, this, [this, client](){
        m_clients.remove(client);
    });
}

void WifiLampApplication::handleRoot(HttpClientConnection *connection, const HttpRequest &request)
{
    QString output = "<h1>Lampen-Steuerung</h1>";

    output.append("<a href=\"/refresh\">Alle aktualisieren</a>");

    output.append("<table border=\"1\">");
    output.append("<thead>");
    output.append("<tr>");
    output.append("<th>IP-Adress</th>");
    output.append("<th>Name</th>");
    output.append("<th>Status</th>");
    output.append("<th>Actions</th>");
    output.append("</tr>");
    output.append("</thead>");
    output.append("<tbody>");

    for(auto client : m_clients)
    {
        output.append("<tr>");
        output.append("<td>" % clientId(client, true).toHtmlEscaped() % "</td>");
        output.append("<td>" % client->name().toHtmlEscaped() % "</td>");
        output.append("<td>" % client->status().toHtmlEscaped() % "</td>");
        output.append("<td>");
        output.append("<a href=\"/devices/" % clientId(client).toHtmlEscaped() % "/toggle\">" % tr("Toggle") % "</a> ");
        output.append("<a href=\"/devices/" % clientId(client).toHtmlEscaped() % "/on\">" % tr("On") % "</a> ");
        output.append("<a href=\"/devices/" % clientId(client).toHtmlEscaped() % "/off\">" % tr("Off") % "</a> ");
        output.append("<a href=\"/devices/" % clientId(client).toHtmlEscaped() % "/refresh\">" % tr("Refresh") % "</a> ");
        output.append("<a href=\"/devices/" % clientId(client).toHtmlEscaped() % "/reboot\">" % tr("Reboot") % "</a> ");
        output.append("<a href=\"/devices/" % clientId(client).toHtmlEscaped() % "/delete\">" % tr("Delete") % "</a> ");
        output.append("</td>");
        output.append("</tr>");
    }

    output.append("</tbody>");
    output.append("</table>");

    HttpResponse response;
    response.protocol = request.protocol;
    response.statusCode = HttpResponse::StatusCode::OK;
    response.headers.insert(HttpResponse::HEADER_SERVER, SERVER_NAME);
    response.headers.insert(HttpResponse::HEADER_CONTENTTYPE, QStringLiteral("text/html"));

    connection->sendResponse(response, output);
}

void WifiLampApplication::redirectRoot(HttpClientConnection *connection, const HttpRequest &request)
{
    HttpResponse response;
    response.protocol = request.protocol;
    response.statusCode = HttpResponse::StatusCode::Found;

    response.headers.insert(HttpResponse::HEADER_SERVER, SERVER_NAME);
    response.headers.insert(HttpResponse::HEADER_CONTENTTYPE, QStringLiteral("text/html"));
    response.headers.insert(HttpResponse::HEADER_LOCATION, QStringLiteral("/"));

    connection->sendResponse(response, "<a href=\"/\">" % tr("Follow this link") % "</a>");
}

QString WifiLampApplication::clientId(const WifiLampClient *client, bool forceIp)
{
    if(!client->name().isEmpty() && !forceIp)
        return client->name();
    return client->peerAddress().toString() % ':' % QString::number(client->peerPort());
}
