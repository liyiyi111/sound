#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include "base.h"
#include "tool/localconfig.h"
#include "funcView.h"

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

    auto trans = new QTranslator(this);
    QJsonArray arr;
    auto fpath = uR"(%1/%2)"_qs.arg(QDir::currentPath()).arg("config.json");
    auto err = loadConfig(fpath,arr);
    for(auto &&arrObj : arr)
    {
        auto obj = arrObj.toObject();
        auto configInfo = obj["configInfo"].toString();
        auto isLoad = trans->load(configInfo);
        assert(isLoad);
        QCoreApplication::installTranslator(trans);
    }
    qDebug() << QDir::currentPath();
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

