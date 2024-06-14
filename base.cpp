#include "base.h"
NetDriver::NetDriver(QObject *parent):
    QObject{parent},
    m_client(new QTcpSocket(this))
{
    connect(m_client,&QTcpSocket::connected,
            this,&NetDriver::readyReadSlot);

    connect(m_client,&QTcpSocket::disconnected,
            this,&NetDriver::netDisConnected);

    m_client->setProxy(QNetworkProxy::NoProxy);
}

NetDriver::~NetDriver()
{
    if(m_client->state() == QTcpSocket::ConnectedState ||
        m_client->state() == QTcpSocket::ConnectingState)
    {
        m_client->disconnectFromHost();
        m_client->waitForDisconnected();
    }
}

bool NetDriver::netConnectToSever(const severInfo& ipWithPort)
{
    if(m_client->state() == QTcpSocket::ConnectedState ||
        m_client->state() == QTcpSocket::ConnectingState)
    {
        m_client->disconnectFromHost();
        bool ok = m_client->waitForDisconnected();
        if(!ok)
            return false;
    }
    m_client->connectToHost(ipWithPort.ip,ipWithPort.port);
    auto state = m_client->waitForConnected(300);
    if(!state)
        qDebug() << m_client->errorString();
    return state;
}

bool NetDriver::netConnectToSever(const QString &ip, const int &port)
{
    if(m_client->state() == QTcpSocket::ConnectedState ||
        m_client->state() == QTcpSocket::ConnectingState)
    {
        m_client->disconnectFromHost();
        bool ok = m_client->waitForDisconnected();
        if(!ok)
            return false;
    }
    auto address = QHostAddress(ip);
    auto converPort = (unsigned short)port;
    m_client->connectToHost(address,converPort);
    auto state = m_client->waitForConnected(300);
    if(!state)
        qDebug() << m_client->errorString();
    return state;
}

void NetDriver::netDisconnectFromServer()
{
    if(m_client->state() == QTcpSocket::ConnectedState ||
        m_client->state() == QTcpSocket::ConnectingState)
    {
        m_client->disconnectFromHost();
    }
}

void NetDriver::netWrite(const QByteArray &data)
{
    auto callLen = m_client->write(data);
    if(callLen != data.length())
        qDebug() << "data length error";
}

void NetDriver::readyReadSlot()
{
    auto data = m_client->readAll();
    emit netRead(data);
}

parse::parse(NetDriver *driver) :
    m_driver(driver)
{
    connect(m_driver,&NetDriver::netRead,this,[this](const QByteArray& array){
        this->m_Datas = std::move(array);
    });
}

parse::~parse()
{

}
