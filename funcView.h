#pragma once

#include <QWidget>
#include <QTranslator>
#include <QComboBox>
#include "base.h"
#include "protocal.h"

namespace Ui {
class funcView;
}

namespace Test
{
#if 0
/**
 * @brief findChildren
 * @param std::remove_pointer<T>::type 移除T类型中的指针 QLabel* -> QLabel;
 * @param std::remove_cv<....>::type 移除T类型中的const和volatile限定符
 * @param objType 为T的基础类型，ifT是const QLabel* objType is QLabel
 */

/**
 * @brief findChildren
 * @param this 在内部查找子控件
 * @param ObjType::staticMetaObject: 传入要查找的控件类型
* @param
 * @return
 */
template<typename T>
QList<T> findChildren(Qt::FindChildOptions options = Qt::FindChildrenRecursively) const
{
    typedef typename std::remove_cv<typename std::remove_pointer<T>::type>::type ObjType;
    QList<T> list;
    qt_qFindChildren_helper(this, ObjType::staticMetaObject,
                            reinterpret_cast<QList<void *> *>(&list), options);
    return list;
}
#endif
template<typename T>
QList<T> findChildren(Qt::FindChildOptions options = Qt::FindChildrenRecursively)
{
    typedef typename std::remove_cv<typename std::remove_pointer<T>::type>::type ObjType;
    QList<T> list;
    qt_qFindChildren_helper(nullptr,ObjType::staticMetaObject,
                            reinterpret_cast<QList<void *>*>(&list),options);
    return list;
}
}

class funcView : public QWidget
{
    Q_OBJECT

public:
    explicit funcView(QWidget *parent = nullptr);
    ~funcView();

    void initView();
    void initConnect();
    void setTitle(QString &ip);
    void setDriver(const NetDriver *driver);
    void sendHostVolume(const int& val);
    void sendPersonNums(const int& val);
    void sendCellSetting(const Parse::CellState& state);
    void sendPTZProtocol(const QString& val);
    void sendBaud(const QString& val);
    void sendSwitchBaud(const QString& val);
    void sendNetWork();
    void sendPTZAddress(const QString& camerId,const QString& val);
    void sendCellAddress(const QString& val);
    void sendNetWorkConfig(const ShardDatas::netWork &config);
    void sendUp(const int& camerId);
    void sendDown(const int& camerId);
    void sendLeft(const int& camerId);
    void sendRight(const int& camerId);
    void sendBlowup(const int& camerId);
    void sendZoomout(const int& camerId);
    [[maybe_unused]]void reboot();

protected:
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    bool event(QEvent *event) override;

private:
    Ui::funcView *ui;
    const NetDriver *m_driver;
    QTranslator *m_trans;
    Parse *m_parse;
    QString m_ip;
    bool m_bisIP;
};
