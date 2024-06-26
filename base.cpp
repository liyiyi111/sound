#include "base.h"
#include <QMessageBox>
#include <QHostAddress>

#define SWITCHVIRTUAL_PORT 0
static const long INIT_DATALENGTH = 11;

static auto tuple = [](const QString& val)
{
    auto ip1 = val.section('.',0,0).toInt();
    auto ip2 = val.section('.',1,1).toInt();
    auto ip3 = val.section('.',2,2).toInt();
    auto ip4 = val.section('.',3,3).toInt();
    return std::make_tuple(ip1, ip2, ip3, ip4);
};

static auto calculateCS = [](const QByteArray &bytes)
{
    quint16 cs = 0;
    auto isExist = bytes.contains(0xFF);
    QByteArray virtualBytes;
    if(isExist)
    {
        auto index = bytes.indexOf(0xFF);
        virtualBytes = bytes.mid(index+1);
        qDebug() << uR"(<font color=blue>([contain(0xFF)]->%1)<font>)"_qs.arg(virtualBytes);
    }
    else
    {
        virtualBytes = std::move(bytes);
        virtualBytes.removeFirst();
        qDebug() << QString("<font color=blue>%1<font>)").arg(virtualBytes);
    }
    for(auto &&byte : virtualBytes)
        cs += static_cast<quint16>(byte);
    return cs;
};

static auto calcCs(const QByteArray &bytes)
{
    quint16 cs = 0;
    auto isExist = bytes.contains(0xFF);
    QByteArray virtualBytes;
    if(isExist)
    {
        auto index = bytes.indexOf(0xFF);
        virtualBytes = bytes.mid(index+1);
        qDebug() << uR"(<font color=blue>([contain(0xFF)]->%1)<font>)"_qs.arg(virtualBytes);
    }
    else
    {
        virtualBytes = std::move(bytes);
        virtualBytes.removeFirst();
        qDebug() << QString("<font color=blue>%1<font>)").arg(virtualBytes);
    }
    for(auto &&byte : virtualBytes)
        cs += static_cast<quint16>(byte);
    return cs;
}

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

std::tuple<int,QString> NetDriver::netConnectToSever(const QString &ip, const int &port)
{
    if(m_client->state() == QTcpSocket::ConnectedState ||
        m_client->state() == QTcpSocket::ConnectingState)
    {
        m_client->disconnectFromHost();
        bool ok = m_client->waitForDisconnected();
        auto errMess = m_client->errorString();
        if(!ok)
            return {false,errMess};
    }
    auto address = QHostAddress(ip);
    auto converPort = (unsigned short)port;
    m_client->connectToHost(address,converPort);
    auto state = m_client->waitForConnected(300);
    if(!state)
    {
        auto errMess = m_client->errorString();
        return {state,errMess};
    }
    return {state,{}};
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
    m_driver(driver),
    m_controller(new protocalController(this))
{
    connect(m_driver,&NetDriver::netRead,this,[this](const QByteArray& array){
        qDebug() << array;
        this->m_Datas = std::move(array);
    });
}

Parse::~Parse()
{
    if(m_base != nullptr)
        delete m_base;
}

void Parse::sendHostVolume(const int &volume)
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append((char)0x00);
    bytes.append(0x01);
    bytes.append(volume);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFE);
    assert(bytes.size() == INIT_DATALENGTH);
    m_driver->netWrite(bytes);
}

void Parse::sendPersonNums(const int &num)
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append((char)0x00);
    bytes.append(0x02);
    bytes.append(num);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFE);
    assert(bytes.size() == INIT_DATALENGTH);
    m_driver->netWrite(bytes);
}

void Parse::cellAddress()
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x01);
    bytes.append(0x01);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == INIT_DATALENGTH);
    m_driver->netWrite(bytes);
}

void Parse::camerAddress()
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x01);
    bytes.append(0x02);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == INIT_DATALENGTH);
    m_driver->netWrite(bytes);
}

void Parse::microphoneGain()
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x01);
    bytes.append(0x03);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == INIT_DATALENGTH);
    m_driver->netWrite(bytes);
}

void Parse::headphoneVolume()
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x01);
    bytes.append(0x04);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == INIT_DATALENGTH);
    m_driver->netWrite(bytes);
}

void Parse::presetPoint()
{

    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x01);
    bytes.append(0x05);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == INIT_DATALENGTH);
    m_driver->netWrite(bytes);
}

void Parse::ipAddress(const QString &val)
{
    if(val.isEmpty()) return;
    auto convert = tuple(val);
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x03);
    bytes.append(0x01);
    bytes.append((char)0x00); //TODO 三级菜单
    bytes.append(std::get<0>(convert));
    bytes.append(std::get<1>(convert));
    bytes.append(std::get<2>(convert));
    bytes.append(std::get<3>(convert));
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == INIT_DATALENGTH);
    m_driver->netWrite(bytes);
}

void Parse::subnetMask(const QString &val)
{
    if(val.isEmpty()) return;
    auto convert = tuple(val);
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x03);
    bytes.append(0x02);
    bytes.append((char)0x00);
    bytes.append(std::get<0>(convert));
    bytes.append(std::get<1>(convert));
    bytes.append(std::get<2>(convert));
    bytes.append(std::get<3>(convert));
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == INIT_DATALENGTH);
    m_driver->netWrite(bytes);
}

void Parse::gateway(const QString &val)
{
    if(val.isEmpty()) return;
    auto convert = tuple(val);
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x03);
    bytes.append(0x03);
    bytes.append((char)0x00);
    bytes.append(std::get<0>(convert));
    bytes.append(std::get<1>(convert));
    bytes.append(std::get<2>(convert));
    bytes.append(std::get<3>(convert));
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == INIT_DATALENGTH);
    m_driver->netWrite(bytes);
}

void Parse::switchLanguage(const QString &val)
{
    QHash<QString,char> boxes = {
        {"简体中文",(char)0x00},
        {"English",0x01}
    };

    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x04);
    bytes.append(boxes[val]);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == INIT_DATALENGTH);
    m_driver->netWrite(bytes);
}

[[maybe_unused]]void Parse::sendCellSetting(const CellState &state)
{
    QByteArray bytes;
    bytes.append(0xFC);
    switch(state)
    {
    case CellState::cell:
        bytes.append(0x01);
        bytes.append((char)0x00);
        bytes.append((char)0x00);
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

void Parse::ptzProtocol(const QString &val)
{

    QHash<QString,char> boxes = {
        {"VISCA",(char)0x00},
        {"PELCO-D",0x02},
        {"PELCO-P",0x03},
    };
#if SWITCHVIRTUAL_PORT
    m_base = m_controller->base(val);
#else
    QHash<QString,Protocol> state = {
        {"VISCA",Protocol::VISCA},
        {"PELCO-D",Protocol::PELCO_D},
        {"PELCO-P",Protocol::PELCO_P},
        };
    m_prostate = state.value(val);
#endif
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x02);
    bytes.append(0x01);
    bytes.append(boxes[val]);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == INIT_DATALENGTH);
    m_driver->netWrite(bytes);
}

void Parse::ptzAddress(const QString &val)
{
    auto intVal = val.toInt();
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x02);
    bytes.append(0x02);
    bytes.append(intVal);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == INIT_DATALENGTH);
    m_driver->netWrite(bytes);
}

void Parse::ptzBaud(const QString &val)
{
    QHash<QString, char> boxes = {
                                  {"2400",(char)0x00},
                                  {"4800",0x01},
                                  {"9600",0x02},
                                  };

    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x03);
    bytes.append(0x03);
    bytes.append(boxes.value(val));
    bytes.append(char(0x00));
    bytes.append(char(0x00));
    bytes.append(char(0x00));
    bytes.append(char(0x00));
    bytes.append(char(0x00));
    bytes.append(char(0x00));
    bytes.append(0xFD);
    assert(bytes.size() == INIT_DATALENGTH);
    m_driver->netWrite(bytes);
}

void Parse::switchBaud(const QString &val)
{
    QHash<QString, char> boxes = {
                                  {"2400",(char)0x00},
                                  {"4800",0x01},
                                  {"9600",0x02},
                                  };
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x03);
    bytes.append(0x04);
    bytes.append(boxes.value(val));
    bytes.append(char(0x00));
    bytes.append(char(0x00));
    bytes.append(char(0x00));
    bytes.append(char(0x00));
    bytes.append(char(0x00));
    bytes.append(char(0x00));
    bytes.append(0xFD);
    assert(bytes.size() == INIT_DATALENGTH);
    m_driver->netWrite(bytes);
}

[[maybe_unused]]void Parse::sendPTZAddress(const QString &camerId, const QString &val)
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

[[maybe_unused]]void Parse::sendCellAddress(const QString &val)
{}

[[maybe_unused]]void Parse::sendNetWorkConfig(const ShardDatas::netWork &config)
{}

void Parse::up(const int& camerId)
{
#if SWITCHVIRTUAL_PORT
    m_base->camerId(camerId);
    auto bytes = m_base->up();
#else
    QByteArray bytes;
    quint16 cs = 0;
    QByteArray copy;
    bytes.append(0xFC);
    switch (m_prostate) {
    case Protocol::VISCA:
        bytes.append(0x80 | (camerId & 0xFF));
        bytes.append(0x01);
        bytes.append(0x06);
        bytes.append(0x01);
        bytes.append(0x1F);
        bytes.append(0x1F);
        bytes.append(0x03);
        bytes.append(0x01);
        bytes.append(0xFF);
        bytes.append(0xFD);
        assert(bytes.size() == INIT_DATALENGTH);
        break;
    case Protocol::PELCO_D:
        bytes.append(0xFF);
        bytes.append(camerId);
        bytes.append((char)0x00);
        bytes.append(0x08);
        bytes.append(0x1F);
        bytes.append(0x1F);
        copy = bytes.mid(2);
        for(auto &&byte : copy)
            cs += static_cast<quint16>(byte);
        bytes.append(cs);
        bytes.append((char)0x00);
        bytes.append((char)0x00);
        bytes.append(0xFD);
        assert(bytes.size() == INIT_DATALENGTH);
        break;
    case Protocol::PELCO_P:
        bytes.append(0xA0);
        bytes.append(camerId); //TODO 加入相机
        bytes.append((char *)0x00);
        bytes.append(0x08);
        bytes.append(0x1F);//VV
        bytes.append(0x1F);//WW
        bytes.append(0xAF);
        for(auto &&byte : copy)
            cs += static_cast<quint16>(byte);
        bytes.append(cs);
        bytes.append((char)0x00);
        bytes.append((char)0x00);
        bytes.append(0xFF);
        assert(bytes.size() == INIT_DATALENGTH);
        break;
    }
#endif
    m_driver->netWrite(bytes);
}

void Parse::down(const int& camerId)
{
    QByteArray bytes;
    quint16 cs = 0;
    QByteArray copy;
    bytes.append(0xFC);
    switch (m_prostate) {
    case Protocol::VISCA:
        bytes.append(0x80 | (camerId & 0xFF));
        bytes.append(0x01);
        bytes.append(0x06);
        bytes.append(0x01);
        bytes.append(0x1F);
        bytes.append(0x1F);
        bytes.append(0x03);
        bytes.append(0x02);
        bytes.append(0xFF);
        bytes.append(0xFD);
        assert(bytes.size() == 11);
        break;
    case Protocol::PELCO_D:
        bytes.append(0xFF);
        bytes.append(camerId);
        bytes.append((char)0x00);
        bytes.append(0x10);
        bytes.append(0x1F);
        bytes.append(0x1F);
        copy = bytes.mid(2);
        for(auto &&byte : copy)
            cs += static_cast<quint16>(byte);
        bytes.append(cs);
        bytes.append((char)0x00);
        bytes.append((char)0x00);
        bytes.append(0xFD);
        assert(bytes.size() == INIT_DATALENGTH);
        break;
    case Protocol::PELCO_P:
        bytes.append(0xA0);
        bytes.append(camerId); //TODO 加入相机
        bytes.append((char *)0x00);
        bytes.append(0x10);
        bytes.append(0x1F);//VV
        bytes.append(0x1F);//WW
        bytes.append(0xAF);
        for(auto &&byte : copy)
            cs += static_cast<quint16>(byte);
        bytes.append(cs);
        bytes.append((char)0x00);
        bytes.append((char)0x00);
        bytes.append(0xFF);
        assert(bytes.size() == INIT_DATALENGTH);
        break;
    }
    m_driver->netWrite(bytes);
}

void Parse::left(const int& camerId)
{
    QByteArray bytes;
    quint16 cs = 0;
    QByteArray copy;
    bytes.append(0xFC);
    switch (m_prostate) {
    case Protocol::VISCA:
        bytes.append(0x80 | (camerId & 0xFF));
        bytes.append(0x01);
        bytes.append(0x06);
        bytes.append(0x01);
        bytes.append(0x1F);
        bytes.append(0x1F);
        bytes.append(0x01);
        bytes.append(0x03);
        bytes.append(0xFF);
        bytes.append(0xFD);
        assert(bytes.size() == 11);
        break;
    case Protocol::PELCO_D:
        bytes.append(0xFF);
        bytes.append(camerId);
        bytes.append((char)0x00);
        bytes.append(0x04);
        bytes.append(0x1F);
        bytes.append(0x1F);
        copy = bytes.mid(2);
        for(auto &&byte : copy)
            cs += static_cast<quint16>(byte);
        bytes.append(cs);
        bytes.append((char)0x00);
        bytes.append((char)0x00);
        bytes.append(0xFD);
        assert(bytes.size() == INIT_DATALENGTH);
        break;
    case Protocol::PELCO_P:
        bytes.append(0xA0);
        bytes.append(camerId); //TODO 加入相机
        bytes.append((char *)0x00);
        bytes.append(0x04);
        bytes.append(0x1F);//VV
        bytes.append(0x1F);//WW
        bytes.append(0xAF);
        for(auto &&byte : copy)
            cs += static_cast<quint16>(byte);
        bytes.append(cs);
        bytes.append((char)0x00);
        bytes.append((char)0x00);
        bytes.append(0xFF);
        assert(bytes.size() == INIT_DATALENGTH);
        break;
    }
    m_driver->netWrite(bytes);
}

void Parse::right(const int& camerId)
{
    QByteArray bytes;
    quint16 cs = 0;
    QByteArray copy;
    bytes.append(0xFC);
    switch (m_prostate) {
    case Protocol::VISCA:
        bytes.append(0x80 | (camerId & 0xFF));
        bytes.append(0x01);
        bytes.append(0x06);
        bytes.append(0x01);
        bytes.append(0x1F);
        bytes.append(0x1F);
        bytes.append(0x02);
        bytes.append(0x03);
        bytes.append(0xFF);
        bytes.append(0xFD);
        assert(bytes.size() == 11);
        break;
    case Protocol::PELCO_D:
        bytes.append(0xFF);
        bytes.append(camerId);
        bytes.append((char)0x00);
        bytes.append(0x02);
        bytes.append(0x1F);
        bytes.append(0x1F);
        copy = bytes.mid(2);
        for(auto &&byte : copy)
            cs += static_cast<quint16>(byte);
        bytes.append(cs);
        bytes.append((char)0x00);
        bytes.append((char)0x00);
        bytes.append(0xFD);
        assert(bytes.size() == INIT_DATALENGTH);
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
    quint16 cs = 0;
    QByteArray copy;
    bytes.append(0xFC);
    switch (m_prostate) {
    case Protocol::VISCA:
        bytes.append(0x80 | (camerId & 0xFF));
        bytes.append(0x01);
        bytes.append(0x04);
        bytes.append(0x07);
        bytes.append(0x02);
        bytes.append(0xFF);
        bytes.append((char)0x00);
        bytes.append((char)0x00);
        bytes.append((char)0x00);
        bytes.append(0xFD);
        assert(bytes.size() == 11);
        break;
    case Protocol::PELCO_D:
        bytes.append(0xFF);
        bytes.append(camerId);
        bytes.append((char)0x00);
        bytes.append(0x40);
        bytes.append(0x1F);
        bytes.append(0x1F);
        copy = bytes.mid(2);
        for(auto &&byte : copy)
            cs += static_cast<quint16>(byte);
        bytes.append(cs);
        bytes.append((char)0x00);
        bytes.append((char)0x00);
        bytes.append(0xFD);
        assert(bytes.size() == INIT_DATALENGTH);
        break;
    case Protocol::PELCO_P:
        bytes.append(0xA0);
        bytes.append(camerId); //TODO 加入相机
        bytes.append((char *)0x00);
        bytes.append(0x40);
        bytes.append(0x1F);//VV
        bytes.append(0x1F);//WW
        bytes.append(0xAF);
        for(auto &&byte : copy)
            cs += static_cast<quint16>(byte);
        bytes.append(cs);
        bytes.append((char)0x00);
        bytes.append((char)0x00);
        bytes.append(0xFF);
        assert(bytes.size() == INIT_DATALENGTH);
        break;
    }
    m_driver->netWrite(bytes);
}

void Parse::zoomOut(const int& camerId)
{
    QByteArray bytes;
    quint16 cs = 0;
    QByteArray copy;
    bytes.append(0xFC);
    switch (m_prostate) {
    case Protocol::VISCA:
        bytes.append(0x80 | (camerId & 0xFF));
        bytes.append(0x01);
        bytes.append(0x04);
        bytes.append(0x07);
        bytes.append(0x03);
        bytes.append(0xFF);
        bytes.append((char)0x00);
        bytes.append((char)0x00);
        bytes.append((char)0x00);
        bytes.append(0xFD);
        assert(bytes.size() == 11);
        break;
    case Protocol::PELCO_D:
        bytes.append(0xFF);
        bytes.append(camerId);
        bytes.append((char)0x00);
        bytes.append(0x20);
        bytes.append(0x1F);
        bytes.append(0x1F);
        copy = bytes.mid(2);
        for(auto &&byte : copy)
            cs += static_cast<quint16>(byte);
        bytes.append(cs);
        bytes.append((char)0x00);
        bytes.append((char)0x00);
        bytes.append(0xFD);
        assert(bytes.size() == INIT_DATALENGTH);
        break;
    case Protocol::PELCO_P:
        bytes.append(0xA0);
        bytes.append(camerId); //TODO 加入相机
        bytes.append((char *)0x00);
        bytes.append(0x20);
        bytes.append(0x1F);//VV
        bytes.append(0x1F);//WW
        bytes.append(0xAF);
        for(auto &&byte : copy)
            cs += static_cast<quint16>(byte);
        bytes.append(cs);
        bytes.append((char)0x00);
        bytes.append((char)0x00);
        bytes.append(0xFF);
        assert(bytes.size() == INIT_DATALENGTH);
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

