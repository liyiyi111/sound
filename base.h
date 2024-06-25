#pragma once
#include "qwindow.h"
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

#include "Tool/TypeList.h"
namespace ShardDatas
{
using namespace Qt;
#define RESIZE_BORDER_SIZE 8 //resize_border_size
#define TITLE_BAR_HEIGHT 32  //title_bar_height
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

static void changeCursorShape(QWidget *view, const QPointF &p)
{
    if (p.x() < RESIZE_BORDER_SIZE && p.y() < RESIZE_BORDER_SIZE || p.x() >= view->width() - RESIZE_BORDER_SIZE && p.y() >= view->height() - RESIZE_BORDER_SIZE) {
        /* Top left or bottom right corner */
        if (view->cursor().shape() != CursorShape::SizeFDiagCursor)
            view->setCursor(QCursor(CursorShape::SizeFDiagCursor));
    } else if (p.x() >= view->width() - RESIZE_BORDER_SIZE && p.y() < RESIZE_BORDER_SIZE || p.x() < RESIZE_BORDER_SIZE && p.y() >= view->height() - RESIZE_BORDER_SIZE) {
        /* Top right or bottom left corner */
        if (view->cursor().shape() != CursorShape::SizeBDiagCursor)
            view->setCursor(QCursor(CursorShape::SizeBDiagCursor));
    } else if (p.x() < RESIZE_BORDER_SIZE || p.x() >= view->width() - RESIZE_BORDER_SIZE) {
        /* Left or right edge */
        if (view->cursor().shape() != CursorShape::SizeHorCursor)
            view->setCursor(QCursor(CursorShape::SizeHorCursor));
    } else if (p.y() < RESIZE_BORDER_SIZE || p.y() >= view->height() - RESIZE_BORDER_SIZE) {
        /* Top or bottom edge */
        if (view->cursor().shape() != CursorShape::SizeVerCursor)
            view->setCursor(QCursor(CursorShape::SizeVerCursor));
    }  else {
        /* Content area */
        if (view->cursor().shape() != CursorShape::ArrowCursor)
            view->setCursor(QCursor(CursorShape::ArrowCursor));
    }
}

static void resizeOrMove(QWidget *view, const QPointF &p)
{
    Edges edges;
    if (p.x() > view->width() - RESIZE_BORDER_SIZE)
        edges |= RightEdge;
    if (p.x() < RESIZE_BORDER_SIZE)
        edges |= LeftEdge;
    if (p.y() < RESIZE_BORDER_SIZE)
        edges |= TopEdge;
    if (p.y() > view->height() - RESIZE_BORDER_SIZE)
        edges |= BottomEdge;

    if (edges != 0) {
        /* Resize the window if the event is triggered inside the borders area */
        if (view->windowHandle()->startSystemResize(edges))
            qInfo() << "[Info] startSystemResize() is supported";
        else
            qInfo() << "[Info] startSystemResize() is NOT supported";
    } else if (p.x() > RESIZE_BORDER_SIZE && p.x() <= view->width() - RESIZE_BORDER_SIZE && p.y() >= RESIZE_BORDER_SIZE && p.y() <= TITLE_BAR_HEIGHT + RESIZE_BORDER_SIZE) {
        /* Move the window if the event is triggered inside the title bar area */
        if (view->windowHandle()->startSystemMove())
            qInfo() << "[Info] startSystemMove() is supported";
        else
            qInfo() << "startSystemMove() is NOT supported";
    }
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
    std::tuple<int,QString> netConnectToSever(const QString& ip, const int& port);
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
    //测试

    explicit Parse(const NetDriver *driver);
    ~Parse();

    void sendHostVolume(const int& volume);
    void sendPersonNums(const int& num);
    //
    void cellAddress();
    void camerAddress();
    void microphoneGain();
    void headphoneVolume();
    void presetPoint();

    void ptzProtocol(const QString& val);
    void ptzAddress(const QString &val);
    void ptzBaud(const QString& val);
    void switchBaud(const QString &val);

    void ipAddress(const QString &val);
    void subnetMask(const QString &val);
    void gateway(const QString &val);

    void switchLanguage(const QString& val);

    [[maybe_unused]]void sendCellSetting(const CellState& state);

    void sendPTZAddress(const QString &camerId, const QString& val);
    void sendCellAddress(const QString& val);
    void sendNetWorkConfig(const ShardDatas::netWork &config);
    //上下左右
    void up(const int& camerId);
    void down(const int& camerId);
    void left(const int& camerId);
    void right(const int& camerId);
    void blowUp(const int& camerId);
    void zoomOut(const int& camerId);

private:
    const NetDriver *m_driver;
    QByteArray m_Datas;
public:
    Protocol m_prostate;
};



