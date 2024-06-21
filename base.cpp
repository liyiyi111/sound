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
{}

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
{}

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
    QByteArray bytes;
    bytes.append(0xFE);
    bytes.append(0xFE);
    bytes.append(0x01);
    bytes.append(volume);
    bytes.append(0xFF);
    m_driver->netWrite(bytes);
}

void Parse::sendPersonNums(const int &num)
{
    QByteArray bytes;
    bytes.append(0xFE);
    bytes.append(0xFE);
    bytes.append(0x02);
    bytes.append(num);
    bytes.append(0xFF);
    m_driver->netWrite(bytes);
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
    QHash<QString,int> boxes = {
        {"VISCA",0x01},//0x00
        {"PELCO-D",0x02}, //0x01
        {"PELCO-P",0x03}, //0x02
    };
    qDebug() << boxes[val];
    QHash<QString,Protocol> state = {
        {"VISCA",Protocol::VISCA},
        {"PELCO-D",Protocol::PELCO_D},
        {"PELCO-P",Protocol::PELCO_P},
    };
    m_prostate = state.value(val);
    QByteArray bytes;
    bytes.append(0xFE);
    bytes.append(0xFE);
    bytes.append(0x04);
    bytes.append(boxes[val]);
    bytes.append(0xFF);
    m_driver->netWrite(bytes);
}

void Parse::sendBaud(const QString &val)
{
    QHash<QString, int> boxes = {
        {"2400",0x01},
        {"9600",0x02}
    };
    QByteArray bytes;
    switch (m_prostate) {
    case Protocol::VISCA:
        bytes.append(0xFE);
        bytes.append(0xFE);
        bytes.append(0x03);
        bytes.append(boxes.value(val));
        bytes.append(0xFF);
        break;
    case Protocol::PELCO_D:
        bytes.append(0xFE);
        bytes.append(0xFE);
        bytes.append(0x03);
        bytes.append(boxes.value(val));
        bytes.append(0xFF);
        break;
    case Protocol::PELCO_P:
        bytes.append(0xFE);
        bytes.append(0xFE);
        bytes.append(0x03);
        bytes.append(boxes.value(val));
        bytes.append(0xFF);
        break;
    }
    m_driver->netWrite(bytes);
}

void Parse::sendPTZAddress(const QString &camerId, const QString &val)
{
    auto intId = camerId.toInt();
    auto intVal = val.toInt();
    QByteArray bytes;
    switch (m_prostate) {
    case Protocol::VISCA:
        bytes.append(intId);
        bytes.append(0x01);
        bytes.append(0x04);
        bytes.append(0x3F);
        bytes.append(0x02);
        bytes.append(intVal);
        bytes.append(0xFF);
        break;
    case Protocol::PELCO_D:
        bytes.append(0xFF);
        bytes.append(intId);
        bytes.append((char *)0x00);
        bytes.append(0x07);
        bytes.append((char *)0x00);
        bytes.append(intVal);
        bytes.append(0xFF);
        break;
    case Protocol::PELCO_P:
        bytes.append(0xA0);
        bytes.append(intId);
        bytes.append((char *)0x00);
        bytes.append(0x07);
        bytes.append((char *)0x00);
        bytes.append(intVal);
        bytes.append(0xAF);
        bytes.append(0xFF);
        break;
    }
    m_driver->netWrite(bytes);
}

void Parse::sendCellAddress(const QString &val)
{}

void Parse::sendNetWorkConfig(const ShardDatas::netWork &config)
{}

void Parse::up(const int& camerId)
{
    QByteArray bytes;
    switch (m_prostate) {
    case Protocol::VISCA:
    bytes.append(camerId);
    bytes.append(0x01); //TODO 加入相机
    bytes.append(0x06);
    bytes.append(0x01);
    bytes.append(0x0F);//VV
    bytes.append(0x0F);//WW
    bytes.append(0xFF);
    break;
    case Protocol::PELCO_D:
    bytes.append(0xFF);
    bytes.append(camerId); //TODO 加入相机
    bytes.append((char *)0x00);
    bytes.append(0x08);
    bytes.append(0x0F);//VV
    bytes.append(0x0F);//WW
    bytes.append(0xFF);
    break;
    case Protocol::PELCO_P:
    bytes.append(0xA0);
    bytes.append(camerId); //TODO 加入相机
    bytes.append((char *)0x00);
    bytes.append(0x08);
    bytes.append(0x0F);//VV
    bytes.append(0x0F);//WW
    bytes.append(0xAF);//WW
    bytes.append(0xFF);
    break;
    }
    m_driver->netWrite(bytes);
}

void Parse::down(const int& camerId)
{
    QByteArray bytes;
    switch (m_prostate) {
    case Protocol::VISCA:
        bytes.append(camerId);
        bytes.append(0x01); //TODO 加入相机
        bytes.append(0x06);
        bytes.append(0x01);
        bytes.append(0x0F);//VV
        bytes.append(0x0F);//WW
        bytes.append(0x03);//WW
        bytes.append(0x01);//WW
        bytes.append(0xFF);
        break;
    case Protocol::PELCO_D:
        bytes.append(0xFF);
        bytes.append(camerId); //TODO 加入相机
        bytes.append((char *)0x00);
        bytes.append(0x08);
        bytes.append(0x0F);//VV
        bytes.append(0x0F);//WW
        bytes.append(0xFF);
        break;
    case Protocol::PELCO_P:
        bytes.append(0xA0);
        bytes.append(camerId); //TODO 加入相机
        bytes.append((char *)0x00);
        bytes.append(0x08);
        bytes.append(0x0F);//VV
        bytes.append(0x0F);//WW
        bytes.append(0xAF);//WW
        bytes.append(0xFF);
        break;
    }
    m_driver->netWrite(bytes);
}

void Parse::left(const int& camerId)
{
    QByteArray bytes;
    switch (m_prostate) {
    case Protocol::VISCA:
        bytes.append(camerId);
        bytes.append(0x01); //TODO 加入相机
        bytes.append(0x06);
        bytes.append(0x01);
        bytes.append(0x0F);//VV
        bytes.append(0x0F);//WW
        bytes.append(0x01);
        bytes.append(0x03);
        bytes.append(0xFF);
        break;
    case Protocol::PELCO_D:
        bytes.append(0xFF);
        bytes.append(camerId); //TODO 加入相机
        bytes.append((char *)0x00);
        bytes.append(0x04);
        bytes.append(0x0F);//VV
        bytes.append(0x0F);//WW
        bytes.append(0xFF);
        break;
    case Protocol::PELCO_P:
        bytes.append(0xA0);
        bytes.append(camerId); //TODO 加入相机
        bytes.append((char *)0x00);
        bytes.append(0x08);
        bytes.append(0x04);
        bytes.append(0x0F);//VV
        bytes.append(0x0F);//WW
        bytes.append(0xAF);
        bytes.append(0xFF);
        break;
    }
    m_driver->netWrite(bytes);
}

void Parse::right(const int& camerId)
{
    QByteArray bytes;
    switch (m_prostate) {
    case Protocol::VISCA:
        bytes.append(camerId);
        bytes.append(0x01); //TODO 加入相机
        bytes.append(0x06);
        bytes.append(0x01);
        bytes.append(0x0F);//VV
        bytes.append(0x0F);//WW
        bytes.append(0x02);
        bytes.append(0x03);
        bytes.append(0xFF);
        break;
    case Protocol::PELCO_D:
        bytes.append(0xFF);
        bytes.append(camerId); //TODO 加入相机
        bytes.append((char *)0x00);
        bytes.append(0x02);
        bytes.append(0x0F);//VV
        bytes.append(0x0F);//WW
        bytes.append(0xFF);
        break;
    case Protocol::PELCO_P:
        bytes.append(0xA0);
        bytes.append(camerId); //TODO 加入相机
        bytes.append((char *)0x00);
        bytes.append(0x02);
        bytes.append(0x0F);//VV
        bytes.append(0x0F);//WW
        bytes.append(0xAF);
        bytes.append(0xFF);
        bytes.append(0xFF);
        break;
    }
    m_driver->netWrite(bytes);
}

void Parse::blowUp(const int& camerId)
{
    QByteArray bytes;
    switch (m_prostate) {
    case Protocol::VISCA:
        bytes.append(0xFF);
        bytes.append(camerId); //TODO 加入相机
        bytes.append((char *)0x00);
        bytes.append(0x04);
        bytes.append(0x0F);//VV
        bytes.append(0x0F);//WW
        bytes.append(0xFF);
        break;
    case Protocol::PELCO_D:
        bytes.append(0xFF);
        bytes.append(camerId); //TODO 加入相机
        bytes.append((char *)0x00);
        bytes.append(0x04);
        bytes.append(0x0F);//VV
        bytes.append(0x0F);//WW
        bytes.append(0xFF);
        break;
    case Protocol::PELCO_P:
        bytes.append(0xFF);
        bytes.append(camerId); //TODO 加入相机
        bytes.append((char *)0x00);
        bytes.append(0x04);
        bytes.append(0x0F);//VV
        bytes.append(0x0F);//WW
        bytes.append(0xFF);
        break;
    }
    m_driver->netWrite(bytes);
}

void Parse::zoomOut(const int& camerId)
{
    QByteArray bytes;
    switch (m_prostate) {
    case Protocol::VISCA:
        bytes.append(0xFF);
        bytes.append(camerId); //TODO 加入相机
        bytes.append((char *)0x00);
        bytes.append(0x40);
        bytes.append(0x0F);//VV
        bytes.append(0x0F);//WW
        bytes.append(0xFF);
        break;
    case Protocol::PELCO_D:
        bytes.append(0xFF);
        bytes.append(camerId); //TODO 加入相机
        bytes.append((char *)0x00);
        bytes.append(0x40);
        bytes.append(0x0F);//VV
        bytes.append(0x0F);//WW
        bytes.append(0xFF);
        break;
    case Protocol::PELCO_P:
        bytes.append(0xFF);
        bytes.append(camerId); //TODO 加入相机
        bytes.append((char *)0x00);
        bytes.append(0x40);
        bytes.append(0x0F);//VV
        bytes.append(0x0F);//WW
        bytes.append(0xFF);
        break;
    }
    m_driver->netWrite(bytes);
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
