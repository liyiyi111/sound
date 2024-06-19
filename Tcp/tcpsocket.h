#pragma once

#include <QTcpSocket>
#include "constant.h"
class TcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit TcpSocket(QObject *parent = nullptr);
    ~TcpSocket();

    void connectTcpSever(const QString& ip,quint16 port);

signals:
    void dataPacketReady(SocketPacket socketData);

public slots:
    void send(QByteArray& data, const QByteArray& dataType);
public slots:
    void onReadyRead();
private:
    bool parsePacket(SocketPacket *packet);

private:
    SocketPacket m_dataPacket;
};
