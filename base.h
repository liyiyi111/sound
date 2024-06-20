#pragma once
#include <QWidget>
#include <QThread>
#include <QtNetwork>
#include <QTcpSocket>
#include <QTcpServer>
#include <QSslSocket>
#include <QPushButton>
#include <QSlider>
#include <QComboBox>
//绘制界面
#include <QPaintEvent>
#include <QStyleOption>
#include <QPainter>
#include <QBitmap>
namespace ShardDatas
{
struct netWork
{
    QString ip;
    QString subnetMask;
    QString gateway;
};

static void drawView(QWidget *view, int w, int h)
{
    QStyleOption opt;
    opt.initFrom(view);
    QPainter p(view);
    view->style()->drawPrimitive(QStyle::PE_Widget,&opt,&p,view);

    QBitmap bmp(view->size());
    bmp.fill();

    QPainter painter(&bmp);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawRoundedRect(bmp.rect(),w,h);
    view->setMask(bmp);
}
}

class PushButton : public QPushButton
{
    Q_OBJECT
public:
    using QPushButton::QPushButton;
protected:
    void paintEvent(QPaintEvent *event) override;
};

class Slider : public QSlider
{
    Q_OBJECT
public:
    using QSlider::QSlider;
protected:
    void paintEvent(QPaintEvent *event) override;
};

class ComboBox : public QComboBox
{
    Q_OBJECT
public:
    using QComboBox::QComboBox;
protected:
    void paintEvent(QPaintEvent *event) override;
};

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
    void netWrite(const QByteArray& data) const;
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

class Parse : public QObject
{
    Q_OBJECT
public:

    enum CellState
    {
        cell,
        voice,
        head
    };

    enum  Protocol
    {
        VISCA = 0,
        PELCO_D,
        PELCO_P
    };

    explicit Parse(const NetDriver *driver);
    ~Parse();

    void sendHostVolume(const int& volume);
    void sendPersonNums(const int& num);
    void sendCellSetting(const CellState& state);
    void sendPTZProtocol(const QString& val);
    void sendBaud(const QString& val);
    void sendPTZAddress(const QString &camerId, const QString& val);
    void sendCellAddress(const QString& val);
    void sendNetWorkConfig(const ShardDatas::netWork &config);
    //上下左右
    void up();
    void down();
    void left();
    void right();
    void blowUp();
    void zoomOut();

private:
    const NetDriver *m_driver;
    QByteArray m_Datas;
public:
    Protocol m_prostate;
};



