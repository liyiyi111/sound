#include "tcpsocket.h"

TcpSocket::TcpSocket(QObject *parent):
    QTcpSocket{parent}
{
    connect(this,&QTcpSocket::readyRead,
            this,&TcpSocket::onReadyRead);
}

TcpSocket::~TcpSocket()
{
    this->disconnectFromHost();
    if(this->state() != QAbstractSocket::UnconnectedState)
        this->waitForDisconnected();
}

void TcpSocket::connectTcpSever(const QString &ip, quint16 port)
{
    this->connectTcpSever(ip,port);
    this->waitForConnected();
}

void TcpSocket::send(QByteArray &data, const QByteArray &dataType)
{

}

void TcpSocket::onReadyRead()
{
    auto data = m_dataPacket.data;
    data += this->readAll();
    //循环解析包数据,m_dataPacket.data中可能不止一包数据
    bool isOk =  false;
    do{
        isOk = parsePacket(&m_dataPacket);
    }while(isOk);
}


bool TcpSocket::parsePacket(SocketPacket *packet)
{
    int pIndexStart = packet->data.indexOf(NET_PACKET_START);
    if(pIndexStart < 0)
        return false;
    //截取从包头index_start到末尾的数据
    packet->data = packet->data.mid(pIndexStart);
    SocketPacket copy;
    copy.data = packet->data;
}
