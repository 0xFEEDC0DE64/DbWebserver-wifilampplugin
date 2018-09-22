#include "wifilampclient.h"

#include <QTcpSocket>

#include "wifilampapplication.h"

WifiLampClient::WifiLampClient(QTcpSocket &socket, WifiLampApplication &application) :
    QObject(&application), m_socket(socket), m_application(application), m_waitingForName(true)
{
    m_socket.setParent(this);

    connect(&m_socket, &QIODevice::readyRead, this, &WifiLampClient::readyRead);
    connect(&m_socket, &QAbstractSocket::disconnected, this, &QObject::deleteLater);
}

quint16 WifiLampClient::localPort() const
{
    return m_socket.localPort();
}

QHostAddress WifiLampClient::localAddress() const
{
    return m_socket.localAddress();
}

quint16 WifiLampClient::peerPort() const
{
    return m_socket.peerPort();
}

QHostAddress WifiLampClient::peerAddress() const
{
    return m_socket.peerAddress();
}

QString WifiLampClient::peerName() const
{
    return m_socket.peerName();
}

const QString &WifiLampClient::name() const
{
    return m_name;
}

const QString &WifiLampClient::status() const
{
    return m_status;
}

void WifiLampClient::on()
{
    m_socket.write(QByteArrayLiteral("1"));
    m_status = QString();
}

void WifiLampClient::off()
{
    m_socket.write(QByteArrayLiteral("0"));
    m_status = QString();
}

void WifiLampClient::toggle()
{
    m_socket.write(QByteArrayLiteral("t"));
    m_status = QString();
}

void WifiLampClient::reboot()
{
    m_socket.write(QByteArrayLiteral("r"));
    m_status = QString();
}

void WifiLampClient::requestStatus()
{
    m_socket.write(QByteArrayLiteral("s"));
    m_status = QString();
}

void WifiLampClient::readyRead()
{
    m_buffer.append(m_socket.readAll());

    int index;
    while((index = m_buffer.indexOf(QByteArrayLiteral("\r\n"))) != -1)
    {
        QString line(m_buffer.left(index));
        m_buffer.remove(0, index + 2);

        if(m_waitingForName)
        {
            const auto iter = std::find_if(m_application.clients().constBegin(), m_application.clients().constEnd(),
                                           [&line](auto client) { return client->name() == line; });
            if(iter != m_application.clients().constEnd())
                delete *iter;

            m_name = line;
            m_waitingForName = false;
        }
        else
            m_status = line;
    }
}
