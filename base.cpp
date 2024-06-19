#include "base.h"
#include <QMessageBox>

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
    {
        auto errMess = m_client->errorString();
        QMessageBox::about(NULL,tr("错误提示"),tr(errMess.toUtf8()));
    }
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

void NetDriver::netGetNetStatus()
{

}

void NetDriver::netWrite(const QByteArray &data) const
{
    auto callLen = m_client->write(data);
    assert(callLen = data.size());
}

void NetDriver::readyReadSlot()
{
    auto data = m_client->readAll();
    emit netRead(data);
}

void NetDriver::callheartbeat()
{

}

Parse::Parse(const NetDriver *driver) :
    m_driver(driver)
{
    connect(m_driver,&NetDriver::netRead,this,[this](const QByteArray& array){
     qDebug() << array;
     this->m_Datas = std::move(array);
    });
}

Parse::~Parse()
{}

void Parse::sendHostVolume(const int &volume)
{

}

void Parse::sendPersonNums(const int &num)
{

}

void Parse::sendCellSetting(const CellState &state)
{
    QByteArray bytes;
    bytes.append(0xA0);
    switch(state)
    {
    case CellState::cell:
        bytes.append((char*)0x00);
        bytes.append((char*)0x00);
        bytes.append((char*)0x00);
        bytes.append(0x0A);
        m_driver->netWrite(std::move(bytes));
        break;
    case CellState::head:
        bytes.append((char*)0xFA);
        bytes.append((char*)0x00);
        bytes.append((char*)0x00);
        bytes.append(0x0A);
        m_driver->netWrite(std::move(bytes));
        break;
    case CellState::voice:
        bytes.append((char*)0x00);
        bytes.append((char*)0x00);
        bytes.append((char*)0x00);
        bytes.append(0x0A);
        m_driver->netWrite(std::move(bytes));
        break;
    }
}

void Parse::sendPTZProtocol(const QString &val)
{

}

void Parse::sendBaud(const QString &val)
{

}

void Parse::sendPTZAddress(const QString &val)
{

}

void Parse::sendCellAddress(const QString &val)
{

}

void Parse::sendNetWorkConfig(const ShardDatas::netWork &config)
{

}

void PushButton::paintEvent(QPaintEvent *event)
{
    ShardDatas::drawView(this,1,1);
    QPushButton::paintEvent(event);
}

void Slider::paintEvent(QPaintEvent *event)
{
    ShardDatas::drawView(this,1,1);
    QSlider::paintEvent(event);
}

void ComboBox::paintEvent(QPaintEvent *event)
{
    ShardDatas::drawView(this,1,1);
    QComboBox::paintEvent(event);
}
