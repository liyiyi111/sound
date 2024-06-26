#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QPoint>

#include <QWindow>
#include <QMouseEvent>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class LineEdit : public QLineEdit
{
    Q_OBJECT
public:
    using QLineEdit::QLineEdit;
protected:
    void paintEvent(QPaintEvent *event) override;
};

using namespace Qt;
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
protected:
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    bool event(QEvent *event) override;

private:
    Ui::MainWindow *ui;
private:
    class Data;
    std::unique_ptr<Data> d;

};
