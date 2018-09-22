#pragma once

#include <QObject>
#include <QHostAddress>

class QTcpSocket;
class WifiLampApplication;

class WifiLampClient : public QObject
{
    Q_OBJECT
public:
    explicit WifiLampClient(QTcpSocket &socket, WifiLampApplication &application);

    quint16 localPort() const;
    QHostAddress localAddress() const;
    quint16 peerPort() const;
    QHostAddress peerAddress() const;
    QString peerName() const;

    const QString &name() const;
    const QString &status() const;

public Q_SLOTS:
    void on();
    void off();
    void toggle();
    void reboot();
    void requestStatus();

private Q_SLOTS:
    void readyRead();

private:
    QTcpSocket &m_socket;
    WifiLampApplication &m_application;
    QByteArray m_buffer;
    bool m_waitingForName;

    QString m_name;
    QString m_status;
};
