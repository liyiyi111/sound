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
    this->setWindowTitle(tr("数字会议系统"));
    this->setMinimumSize(QSize(1280,720));
    initView();

    connect(ui->btnReboot,&QPushButton::clicked,this,&funcView::reboot);
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

        auto groups = new QButtonGroup(this);
        for(auto &[btn, id] : btns)
        {
            btn->setText(boxes.value(id));
            groups->addButton(btn,id);
        }

        connect(groups,&QButtonGroup::idClicked,
                this,[groups,widget](const int& id){
            for(auto &&btn : groups->buttons())
                btn->setStyleSheet("background-color:rgb(144,156,165);");
            groups->button(id)->setStyleSheet("background-color:rgb(28,156,165);");
            widget->setCurrentIndex(id);
        });
        btns.at(0).first->click();
    };
    btnGroups({{ui->btnHost,0},{ui->btnCell,1},{ui->btnTrack,2},
               {ui->btnLanguage,3},{ui->btnNetwork,4}},ui->stackedWidget);

    QList<QLabel *> labelList = findChildren<QLabel *>();
    for (QLabel *label : labelList) {
        auto text = label->text().toUtf8();
        label->setText(tr(text));
    }

    auto buttons = findChildren<QPushButton *>();
    for(QPushButton *button : buttons)
    {
        auto text  = button->text().toUtf8();
        button->setText(tr(text));
    }

    std::map<QString,QString> items = {
        {"简体中文","zh_CN"},
        {"English","en_UN"},
        };

    for(auto &&[text,value]: items)
        ui->comLanguage->addItem(text,value);

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

}

void funcView::setDriver(const NetDriver *driver)
{
    assert(driver);
    m_driver = driver;
}

void funcView::reboot()
{
    auto re = [this](){
        auto program = QApplication::applicationFilePath();
        auto arguments = QApplication::arguments();
        auto workingDirectory = QDir::currentPath();
        qDebug() << program;
        qDebug() << arguments;
        qDebug() << workingDirectory;
         /*
         * Starts the program set by setProgram() with arguments set by setArguments() in a new process, and detaches from it.
         * Returns true on success; otherwise returns false.
         * If the calling process exits, the detached process will continue to run unaffected.
         * */
        QProcess::startDetached(program,arguments,workingDirectory);
        //Tells the application to exit with a return code.
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

