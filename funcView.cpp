#include "funcView.h"
#include "ui_funcView.h"

#include <QButtonGroup>
#include <QTranslator>

funcView::funcView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::funcView)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("数字会议系统"));
    this->setMinimumSize(QSize(1280,720));
    initView();
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
        label->setText(QObject::tr(text));
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
        auto trans = new QTranslator(this);
        auto model = ui->comLanguage->itemData(index).toString();
        bool ok = trans->load(":/" + model + ".qm");
        assert(ok);
        QCoreApplication::installTranslator(trans);
    });

}

void funcView::setDriver(const NetDriver *driver)
{
    assert(driver);
    m_driver = driver;
}

