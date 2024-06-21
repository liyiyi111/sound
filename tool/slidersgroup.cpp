#include "slidersgroup.h"
#include <QBoxLayout>
#include <QDial>
#include <QScrollBar>
#include <QSlider>


namespace Tools
{

SlidersGroup::SlidersGroup(Qt::Orientation orientation, const QString &title,
                           QWidget *parent)
    : QGroupBox(title,parent)
{
    slider = new QSlider(orientation);
    //这行代码设置slider的焦点策略
    slider->setFocusPolicy(Qt::StrongFocus);
    slider->setTickPosition(QSlider::TicksBothSides);
    //设置slider的刻度间隔为10
    slider->setTickInterval(10);
    //设置滑块控件单步步长
    slider->setSingleStep(1);

    scrollBar = new QScrollBar(orientation);
    scrollBar->setFocusPolicy(Qt::StrongFocus);

    dial = new QDial;
    dial->setFocusPolicy(Qt::StrongFocus);

    connect(slider, &QSlider::valueChanged, scrollBar, &QScrollBar::setValue);
    connect(scrollBar, &QScrollBar::valueChanged, dial, &QDial::setValue);
    connect(dial, &QDial::valueChanged, slider, &QSlider::setValue);
    //! [0] //! [1]
    connect(dial, &QDial::valueChanged, this, &SlidersGroup::valueChanged);

    QBoxLayout::Direction direction;

    if(orientation == Qt::Horizontal)
        direction = QBoxLayout::TopToBottom;
    else
        direction = QBoxLayout::LeftToRight;

    QBoxLayout *slidersLayout = new QBoxLayout(direction);
    slidersLayout->addWidget(slider);
    slidersLayout->addWidget(scrollBar);
    slidersLayout->addWidget(dial);
    setLayout(slidersLayout);

    QString styleSheet = "QSlider::handle {"
                         "width: 200%;"
                         "height: 20px;"
                         "background-color: #ff0000;"
                         "border-radius: 10px;"
                         "border-image:url(:/推子.png);"
                         "}"
                         "QSlider::groove::vertical {"
                         "backgroud:#1E2540;"
                         "}";

    slider->setStyleSheet(styleSheet);


}

void SlidersGroup::setValue(int value)
//! [5] //! [6]
{
    slider->setValue(value);
}
//! [6]

//! [7]
void SlidersGroup::setMinimum(int value)
//! [7] //! [8]
{
    slider->setMinimum(value);
    scrollBar->setMinimum(value);
    dial->setMinimum(value);
}
//! [8]

//! [9]
void SlidersGroup::setMaximum(int value)
//! [9] //! [10]
{
    slider->setMaximum(value);
    scrollBar->setMaximum(value);
    dial->setMaximum(value);
}
//! [10]

//! [11]
void SlidersGroup::invertAppearance(bool invert)
//! [11] //! [12]
{
    slider->setInvertedAppearance(invert);
    scrollBar->setInvertedAppearance(invert);
    dial->setInvertedAppearance(invert);
}
//! [12]

//! [13]
void SlidersGroup::invertKeyBindings(bool invert)
//! [13] //! [14]
{
    slider->setInvertedControls(invert);
    scrollBar->setInvertedControls(invert);
    dial->setInvertedControls(invert);
}
//! [14]
}// namespace Tools
