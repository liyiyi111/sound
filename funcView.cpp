#include "funcView.h"
#include "ui_funcView.h"

#include <QButtonGroup>
#include <QTranslator>
#include <QMessageBox>



static QString saveConfigImp(const QString &fileName, const QByteArray &byte)
{
    QFile f(fileName);
    if(!f.open(QFile::WriteOnly) || f.write(byte.data(), byte.size()) == -1)
        return f.errorString();
    return {};
}

static QString saveConfigImp(const QString &fileName, const QJsonDocument &doc)
{
    return saveConfigImp(fileName, doc.toJson());
}

static QString saveConfigImp(const QString &fileName, const QJsonArray &arrs)
{
    return saveConfigImp(fileName, QJsonDocument(arrs));
}

funcView::funcView(QWidget *parent):
    QWidget(parent),
    ui(new Ui::funcView),
    m_trans(new QTranslator(this))
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setMinimumSize(QSize(1280,720));
    initView();

    std::map<QString,QString> items = {
                                        {"简体中文","zh_CN"},
                                        {"English","en_UN"},
                                    };
    for(auto &&[text,value]: items)
        ui->comLanguage->addItem(text,value);
    ui->comLanguage->setCurrentIndex(1);

    //不重启切换
    connect(ui->comLanguage,&QComboBox::currentIndexChanged,
            this, [this,items](const int& index){
                auto model = ui->comLanguage->itemData(index).toString();
                auto convert = uR"(:/%1.qm)"_qs.arg(model);
                auto isLoad = m_trans->load(convert);
                assert(isLoad);
                QCoreApplication::installTranslator(m_trans);
                ui->retranslateUi(this);
                initView();
            });
    //单元设置
    auto newGroups = [this](QList<std::pair<QPushButton *,int>>btns) {
        for(auto &[btn,id] : btns)
        {
            btn->setFixedSize(161,55);
            connect(btn,&QPushButton::clicked,this,[this](){
                QMessageBox::about(this,tr("通知"),tr("请通过对应会议单元的按键修改参数"));
            });
        }
    };

    newGroups({{ui->btnCellAddr,0},{ui->btnMic,1},{ui->btnHead,2}});
    //圆角
    auto roundness = [this](QList<std::pair<QPushButton *,int>>btns)
    {
        for(auto &[btn, id] : btns)
        {
            btn->setFixedSize(55,55);
            connect(btn,&QPushButton::clicked,[this,id](){
                this->sendPersonNums(id+1);
            });
        }
        //增加信号发送
    };
    roundness({{ui->button0,0},{ui->button1,1},{ui->button2,2},{ui->button3,3},
               {ui->button4,4},{ui->button5,5}});

    //设置btn图标
    auto newIcon = [this](QList<std::pair<QPushButton *,int>> btns)
    {
        QHash<int,QString> iconBoxes = {
                                         {0,":/上.png"},
                                         {1,":/下.png"},
                                         {2,":/左.png"},
                                         {3,":/右.png"},
                                         };
        QHash<int,QString> iconPressed = {
                                           {0,":/上-选中.png"},
                                           {1,":/下-选中.png"},
                                           {2,":/左-选中.png"},
                                           {3,":/右-选中.png"},
                                           };
        for(auto &[btn,id] : btns)
        {
            QIcon icon;
            icon.addFile(iconBoxes[id]);
            btn->setMinimumSize(90,90);
            btn->setFlat(true);
            btn->setIcon(icon);
            btn->setIconSize(QSize(90,90));
            connect(btn,&QPushButton::pressed,[btn,id,iconPressed](){
                btn->setIcon(QIcon(iconPressed[id]));
            });
            connect(btn,&QPushButton::released,[btn,id,iconBoxes](){
                btn->setIcon(QIcon(iconBoxes[id]));
            });
            switch (id) {
            case 0:
                connect(btn,&QPushButton::clicked,this,&funcView::sendUp);
                break;
            case 1:
                connect(btn,&QPushButton::clicked,this,&funcView::sendDown);
                break;
            case 2:
                connect(btn,&QPushButton::clicked,this,&funcView::sendLeft);
                break;
            case 3:
                connect(btn,&QPushButton::clicked,this,&funcView::sendRight);
                break;
            }
        }
    };
    newIcon({{ui->btnUp,0},{ui->btnDown,1},{ui->btnLeft,2},{ui->btnRight,3}});


    static const auto moveY = ui->hostWidget->geometry().y() + 80;
    ui->hostWidget->move(ui->hostWidget->geometry().x(), moveY);

    connect(ui->pushButton,&QPushButton::clicked,this, &funcView::close);
    connect(ui->btnBig,&QPushButton::clicked,this,&funcView::sendBlowup);
    connect(ui->btnReduce,&QPushButton::clicked,this,&funcView::sendZoomout);
    initConnect();

    //更新
    for(auto i = 1 ; i <= 16; i++)
        ui->cbCamera->addItem(QString::number(i));

    QString styleSheet = "QSlider::handle {"
                         "width: 200%;"
                         "height: 20px;"
                         "background-color: #ff0000;"
                         "border-radius: 10px;"
                         "border-image:url(:/推子.png);"
                         "}";


    ui->sliderHost->setRange(0,32);
    ui->sliderHost->setStyleSheet(styleSheet);

}

funcView::~funcView()
{
    delete ui;
}

void funcView::initView()
{
    this->setWindowTitle(m_ip);
    auto btnGroups = [this](QList<std::pair<QPushButton*,int>> btns,QStackedWidget *widget){
        QHash<int,QString> boxes = {
                                     {0,tr("主机设置")},
                                     {1,tr("单元设置")},
                                     {2,tr("摄像设置")},
                                     {3,tr("语言设置")},
                                     {4,tr("网络设置")},
                                     };
        QHash<int,QString> icons = {
                                     {0,":/主机.png"},
                                     {1,":/单元.png"},
                                     {2,":/摄像.png"},
                                     {3,":/语言.png"},
                                     {4,":/网络.png"},
                                     };

        auto groups = new QButtonGroup(this);
        groups->setObjectName("mainGroups");
        groups->setExclusive(true);
        for(auto &[btn, id] : btns)
        {
            QIcon icon;
            icon.addFile(icons[id]);
            btn->setIcon(icon);
            btn->setText(boxes.value(id));
            btn->setIconSize(QSize(60,30));
            btn->setMinimumSize(256,50);
            groups->addButton(btn,id);
        }
        connect(groups,&QButtonGroup::idClicked,
                this,[groups,widget](const int& id){
                for(auto &button : groups->buttons())
                    button->setStyleSheet("border-image:url(:/按钮-未选中.png);");
                auto btn  = groups->button(id);
                btn->setStyleSheet("border-image:url(:/按钮-选中.png);");
                widget->setCurrentIndex(id);
                });
        btns.at(0).first->click();
    };
    btnGroups({{ui->btnHost,0},{ui->btnCell,1},{ui->btnTrack,2},
               {ui->btnLanguage,3},{ui->btnNetwork,4}},ui->stackedWidget);

    QList<QLabel *> labelList = findChildren<QLabel *>();
    std::map<QString, QString> labelBoxes = {
                                             {"lbIp", tr("IP地址")},
                                             {"lbgateway", tr("子网掩码")},
                                             {"lbMask", tr("IP网关")},
                                             {"lbLaugu", tr("语言")},
                                             {"lbAddr", tr("云台地址")},
                                             {"lbprotocol", tr("云台协议")},
                                             {"lbCell", tr("单元地址")},
                                             {"lbBaud", tr("波特率")},
                                             {"lbCell2", tr("单元地址")},
                                             {"lbMic", tr("话筒音量")},
                                             {"lbHead", tr("耳机音量")},
                                             {"lbHost", tr("主机音量")},
                                             {"lbNum", tr("发言人数")},
                                             };

    for (QLabel *label : labelList) {
        if (labelBoxes.count(label->objectName())) {
            label->setText(labelBoxes[label->objectName()]);
        }
    }

    auto buttons = findChildren<QPushButton *>();
    for(QPushButton *button : buttons)
    {
        auto text  = button->text().toUtf8();
        button->setText(tr(text));
    }

//重启切换
#if 0
    connect(ui->comLanguage,&QComboBox::currentIndexChanged,
            this, [this](const int& index){
        ui->comLanguage->setCurrentIndex(index);

        auto fpath = uR"(%1/%2)"_qs.arg(QDir::currentPath()).arg("config.json");
        auto model = ui->comLanguage->itemData(index).toString();
        auto convert = uR"(:/%1.qm)"_qs.arg(model);
        QJsonObject obj;
        obj.insert("configInfo",convert);

        QJsonArray arr;
        arr.push_back(obj);
        auto err = saveConfigImp(fpath,arr);
    });
#endif

}

void funcView::initConnect()
{
    connect(ui->sliderHost,&QSlider::valueChanged,this,
            &funcView::sendHostVolume);

    connect(ui->cbProtocol,&QComboBox::currentTextChanged,
            this,&funcView::sendPTZProtocol);

    connect(ui->cbBaud,&QComboBox::currentTextChanged,
            this,&funcView::sendBaud);

}

void funcView::setTitle(QString &ip)
{
    auto size = ip.size();
    m_ip = std::move(ip);
    assert(m_ip.size()  == size);
    this->setWindowTitle(m_ip);
}

void funcView::setDriver(const NetDriver *driver)
{
    assert(driver);
    m_driver = driver;
    m_parse = new Parse(m_driver);
    m_parse->m_prostate = Parse::VISCA;
    connect(ui->cbAddress,&QComboBox::currentTextChanged,
            this, [this](const QString& address){
                auto camerId = ui->cbCamera->currentText();
                sendPTZAddress(camerId, address);
            });

    connect(ui->cbCamera,&QComboBox::currentTextChanged,
            this,[this](const QString& camerId){
                auto address = ui->cbAddress->currentText();
                sendPTZAddress(camerId, address);
            });
}

void funcView::sendHostVolume(const int &val)
{
    m_parse->sendHostVolume(val);
}

void funcView::sendPersonNums(const int &val)
{
    m_parse->sendPersonNums(val);
}

void funcView::sendCellSetting(const Parse::CellState &state)
{
    m_parse->sendCellSetting(state);
}

void funcView::sendPTZProtocol(const QString &val)
{
    m_parse->sendPTZProtocol(val);
}

void funcView::sendBaud(const QString &val)
{
    m_parse->sendBaud(val);
}

void funcView::sendPTZAddress(const QString& camerId,const QString &val)
{
    m_parse->sendPTZAddress(camerId,val);
}

void funcView::sendCellAddress(const QString &val)
{
    m_parse->sendCellAddress(val);
}

void funcView::sendNetWorkConfig(const ShardDatas::netWork &config)
{
    m_parse->sendNetWorkConfig(config);
}

void funcView::sendUp(const int& camerId)
{
    m_parse->up(camerId);
}

void funcView::sendDown(const int& camerId)
{
    m_parse->down(camerId);
}

void funcView::sendLeft(const int& camerId)
{
    m_parse->left(camerId);
}

void funcView::sendRight(const int& camerId)
{
    m_parse->right(camerId);
}

void funcView::sendBlowup(const int& camerId)
{
    m_parse->blowUp(camerId);
}

void funcView::sendZoomout(const int& camerId)
{
    m_parse->zoomOut(camerId);
}

void funcView::reboot()
{
    auto re = [](){
        auto program = QApplication::applicationFilePath();
        auto arguments = QApplication::arguments();
        auto workingDirectory = QDir::currentPath();
        QProcess::startDetached(program,arguments,workingDirectory);
        QApplication::exit();
    };

    QMessageBox messagebox(QMessageBox::NoIcon,"提示","确定是否重启切换语言",
                           QMessageBox::Yes | QMessageBox::No);

    auto result = messagebox.exec();
    switch (result) {
    case QMessageBox::Yes:
        re();
        break;
    case QMessageBox::No:
        qDebug() << "不重启程序";
        break;
    }
}

void funcView::paintEvent(QPaintEvent *event)
{
    ShardDatas::drawView(this,30,30);
    QWidget::paintEvent(event);
}

void funcView::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
}

