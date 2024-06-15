#pragma once
#include <QWidget>
#include <QThread>
#include <QtNetwork>
#include <QTcpSocket>
#include <QTcpServer>
#include <QSslSocket>

class NetDriver : public QObject
{
    Q_OBJECT
public:
    explicit NetDriver(QObject *parent = nullptr);
    ~NetDriver();
public:
    struct severInfo{
        QString ip;
        int port;
    };
    //连接
    bool netConnectToSever(const severInfo& ipWithPort);
    //连接
    bool netConnectToSever(const QString& ip, const int& port);
    //断开
    void netDisconnectFromServer();
    //获取与服务器的连接状态
    void netGetNetStatus();
    //通过网络发送数据
    void netWrite(const QByteArray& data);
    //接收
    void readyReadSlot();
    //TODO 增加心跳回调
    void callheartbeat();
signals:
    void netRead(const QByteArray& data);
    void netDisConnected();
private:
    QTcpSocket *m_client;
};

class parse : public QObject
{
    Q_OBJECT
public:
    explicit parse(NetDriver *driver);
    ~parse();

    void setVolume(const int& volume);
    void setNum(const int& num);


private:
    NetDriver *m_driver;
    QByteArray m_Datas;
};



