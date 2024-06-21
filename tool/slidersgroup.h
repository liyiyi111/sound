#pragma once
#include <QObject>
#include <QGroupBox>
#include <QSlider>
#include <QScrollBar>
#include <QDial>
namespace Tools
{

class SlidersGroup : public QGroupBox
{
    Q_OBJECT
public:
    SlidersGroup(Qt::Orientation orentation, const QString &title,
                 QWidget *parent = nullptr);
signals:
    void valueChanged(int value);
public slots:
    void setValue(int value);
    void setMinimum(int value);
    void setMaximum(int value);
    void invertAppearance(bool invert);
    void invertKeyBindings(bool invert);
private:
    QSlider *slider;
    QScrollBar *scrollBar;
    QDial *dial;
};

}
