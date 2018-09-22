#include "wifilampapplication.h"

#include "httprequest.h"
#include "httpresponse.h"
#include "httpclientconnection.h"

WifiLampApplication::WifiLampApplication(const QJsonObject &config, QObject *parent) :
    WebApplication(parent)
{

}

void WifiLampApplication::start()
{

}

void WifiLampApplication::handleRequest(HttpClientConnection *connection, const HttpRequest &request)
{
    HttpResponse response;
    response.protocol = request.protocol;
    response.statusCode = HttpResponse::StatusCode::OK;
    connection->sendResponse(response, "Hello from WifiLampApplication: " + request.path);
}
