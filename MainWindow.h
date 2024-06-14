#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    enum connentState{
        succ = true,
        fail = false
    };

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void find();

private:
    Ui::MainWindow *ui;
private:
    class Data;
    std::unique_ptr<Data> d;
};
