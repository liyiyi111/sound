#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include "base.h"
#include "funcView.h"

static const quint16 port = 6000;
static QString loadConfig(const QString &fileName, QJsonArray &arrs)
{
    QFile f(fileName);
    if(!f.open(QFile::ReadOnly)) return f.errorString();

    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(f.readAll(), &error);
    arrs = doc.array();
    if(arrs.isEmpty())
        return error.errorString();
    return {};
}

static QString loadCss(const QString& fileName,QByteArray &array)
{
    QFile f(fileName);
    if(!f.open(QFile::ReadOnly))
        return f.errorString();
    array = f.readAll();
    if(array.isEmpty())
        return u"array is Empty"_qs;
    return {};
}

class MainWindow::Data
{
public:
    Data(MainWindow *q);
    ~Data();
public:
    MainWindow *q;
    NetDriver *netDriver;
    QByteArray array;
};

MainWindow::Data::Data(MainWindow *q):
    q(q),
    netDriver(new NetDriver(q)),
    array([](){
        QByteArray array;
        auto err = loadCss(u":/style.css"_qs,array);
        assert(err.isEmpty());
        return array;
    }())
{}

MainWindow::Data::~Data()
{}

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
    , d(std::make_unique<Data>(this))
{
    ui->setupUi(this);
    ui->editIp->setText("192.168.100.6");
    ui->btnFind->setText(tr("搜索"));

    connect(ui->btnFind,&QPushButton::clicked,
            this, &MainWindow::find);
    this->setStyleSheet(d->array);

    //可以设置为最小
    this->setFixedSize(QSize(1280,720));
    ui->widget->setFixedSize(QSize(1280,720));
    ui->editIp->setFixedSize(QSize(219,105));
    ui->btnFind->setFixedSize(QSize(161,52));

    connect(ui->pushButton,&QPushButton::clicked,this,
            &MainWindow::close);

    connect(ui->btnMin,&QPushButton::clicked,
            this, &MainWindow::showMinimized);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::find()
{
    auto view = new funcView();
    auto ip = ui->editIp->text();
    auto tuple = d->netDriver->netConnectToSever(ip,port);
    switch (std::get<0>(tuple)) {
    case connentState::succ:
        this->setVisible(false);
        view->setStyleSheet(d->array);
        view->setTitle(ip);
        view->show();
        view->setDriver(d->netDriver);
        break;
    case connentState::fail:
        QMessageBox::about(this,tr("错误"),tr(std::get<1>(tuple).toUtf8()));
        delete view;
        break;
    }
}

/*
 * 增加QWidget的样式
 * */
void MainWindow::paintEvent(QPaintEvent *event)
{
    ShardDatas::drawView(this,30,30);
    QWidget::paintEvent(event);
}

void LineEdit::paintEvent(QPaintEvent *event)
{
    ShardDatas::drawView(this,5,5);
    QLineEdit::paintEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
}


bool MainWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::HoverMove:
        ShardDatas::changeCursorShape(this,static_cast<QHoverEvent *>(event)->position());
        return true;
    case QEvent::MouseButtonPress:
        ShardDatas::resizeOrMove(this,static_cast<QMouseEvent *>(event)->position());
        return true;
    default:
        return QWidget::event(event);
    }
}

