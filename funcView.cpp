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
        auto grounp = new QButtonGroup(this);
        for(auto &[btn,id] : btns)
        {
            btn->setFixedSize(161,55);
            btn->setFlat(true);
            grounp->addButton(btn,id);
            connect(btn,&QPushButton::clicked,this,[this](){
                QMessageBox::information(this,tr("通知"),tr("<font color=blue>请通过对应会议单元的按键修改参数</font>"));
            });
        }
        connect(grounp, &QButtonGroup::idClicked, this, [this](const int &id) {
            switch (id) {
            case 0:
                m_parse->cellAddress();
                break;
            case 1:
                m_parse->camerAddress();
                break;
            case 2:
                m_parse->microphoneGain();
                break;
            case 3:
                m_parse->headphoneVolume();
                break;
            case 4:
                m_parse->presetPoint();
                break;
            }
        });
    };

    newGroups({{ui->btnCellAddr,0},{ui->btnCamAddr,1},{ui->btnMic,2},{ui->btnHead,3},{ui->btnPoint,4}});
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
    connect(ui->btnMin,&QPushButton::clicked,this, &funcView::showMinimized);
    initConnect();

    //更新
    for(auto i = 1 ; i <= 16; i++)
        ui->cbCamera->addItem(QString::number(i));

    for(auto i = 1;  i <= 128; i++)
        ui->cbAddress->addItem(QString::number(i));

    ui->sliderHost->setRange(0,32);
    ui->sliderHost->setStyleSheet(uR"(
    QSlider {
        background: transparent;
    }
    QSlider::add-page:vertical {
        background-color: #112C47;
        width: 4px;
        border-radius: 2px;
    }
    QSlider::sub-page:vertical {
        background-color: rgba(196, 196, 196, 0.5);
        width: 4px;
        border-radius: 2px;
    }
    QSlider::groove:vertical {
        background: transparent;
        width: 4px;
        border-radius: 2px;
    }
    QSlider::handle:vertical {
        height: 55px;
        width: 70px;
        border-image: url(:/推子.png);
        margin: -0 -10px;
    }
    QSlider::handle:vertical:hover {
        height: 55px;
        width: 70px;
        border-image: url(:/推子.png);
        margin: -0 -10px;
    }
)"_qs);


}

funcView::~funcView()
{
    delete ui;
}

void funcView::initView()
{
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
        //This property holds whether the button group is exclusive   exlusive 独有的
        groups->setExclusive(true);
        for(auto &[btn, id] : btns)
        {
            QIcon icon;
            icon.addFile(icons[id]);
            btn->setIcon(icon);
            btn->setText(boxes[id]);
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

    if(m_bisIP) {ui->cbxIp->setText(m_ip);}

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

    connect(ui->cbSwitchBaud,&QComboBox::currentTextChanged,
            this,&funcView::sendSwitchBaud);

    connect(ui->btnSave2, &QPushButton::clicked,
            this, &funcView::sendNetWork);
}

void funcView::setTitle(QString &ip)
{
    m_bisIP = true;
    m_ip = std::move(ip);
    ui->cbxIp->setText(m_ip);
    ui->cbxIp->setChecked(true);
}

void funcView::setDriver(const NetDriver *driver)
{
    assert(driver);
    m_driver = std::move(driver);
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
    m_parse->ptzProtocol(val);
}

void funcView::sendBaud(const QString &val)
{
    m_parse->ptzBaud(val);
}

void funcView::sendSwitchBaud(const QString &val)
{
    m_parse->switchBaud(val);
}

void funcView::sendNetWork()
{
    auto ip = ui->editIpAddress->text();
    auto submask = ui->editMask->text();
    auto gateway = ui->editGateWay->text();
    m_parse->ipAddress(ip);
    m_parse->subnetMask(submask);
    m_parse->gateway(gateway);
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

bool funcView::event(QEvent *event)
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

