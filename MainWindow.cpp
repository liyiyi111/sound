#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include "base.h"
#include "tool/localconfig.h"
#include "funcView.h"

using namespace Tools::LocalConfig;

class MainWindow::Data
{
public:
    Data(MainWindow *q);
    ~Data();
public:
    MainWindow *q;
    NetDriver *netDriver;
};

MainWindow::Data::Data(MainWindow *q):
    q(q),
    netDriver(new NetDriver(q))
{}

MainWindow::Data::~Data()
{}

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
    , d(std::make_unique<Data>(this))
{
    ui->setupUi(this);
    ui->editIp->setText("192.168.1.244");
    ui->btnFind->setText(tr("搜索"));
    this->setWindowTitle(tr("数字会议系统"));
    connect(ui->btnFind,&QPushButton::clicked,
            this, &MainWindow::find);

    QString fpath = ":/config.json";
    auto trans = new QTranslator(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::find()
{
    auto view = new funcView();

    auto ip = ui->editIp->text();
    int state = d->netDriver->netConnectToSever(ip,5000);
    this->setVisible(false);
    view->show();
    view->setDriver(d->netDriver);

    switch (state) {
    case connentState::succ:
        ui->editIp->setStyleSheet("background-color:green");
        break;
    case connentState::fail:
        ui->editIp->setStyleSheet("background-color:red");
        break;
    }
}

