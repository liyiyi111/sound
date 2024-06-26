#pragma once
#include <QObject>
#include <QHash>
#include "Tool/virtualTemp.h"

class protocalBase;
class protocalController : public QObject
{
    Q_OBJECT
public:
    explicit protocalController(QObject *parent  = nullptr);
    ~protocalController();
    protocalBase *base(const QString &protocalName) const;
private:
    void initStandard();
private:
    QHash<QString, protocalBase*> m_base;
};

class protocalBase : public QObject
{
    Q_OBJECT
public:
    explicit protocalBase(QObject *parent = nullptr)
        :QObject(parent) {}
    virtual ~protocalBase() {

    }
    virtual QString       name() const = 0;
    virtual QByteArray swit()    const = 0;
    virtual int        camerId(const int& camerId) = 0;
    virtual quint16    vv(const quint16& vv)       = 0;
    virtual quint16    ww(const quint16& ww)       = 0;
    virtual QByteArray up()      const = 0;
    virtual QByteArray down()    const = 0;
    virtual QByteArray left()    const = 0;
    virtual QByteArray right()   const = 0;
    virtual QByteArray bigger()  const = 0;
    virtual QByteArray smaller() const = 0;
    virtual QByteArray stop()    const = 0;
    virtual QByteArray clearPoint() const= 0;
    virtual QByteArray setPoint() const= 0;
    virtual QByteArray turnPoint() const= 0;
};

class VISCAProtocal final : public protocalBase
{
    Q_OBJECT
public:
    explicit VISCAProtocal(QObject *parent = nullptr)
        : protocalBase(parent) {}
    ~VISCAProtocal() override;

    QString name()         const override;
    QByteArray swit()      const override;
    int  camerId(const int &camerId)    override;
    quint16    vv(const quint16& vv)    override;
    quint16    ww(const quint16&  ww)   override;
    QByteArray up()        const override;
    QByteArray down()      const override;
    QByteArray left()      const override;
    QByteArray right()     const override;
    QByteArray bigger()    const override;
    QByteArray smaller()   const override;
    QByteArray stop()      const override;
    QByteArray clearPoint()const override;
    QByteArray setPoint()  const override;
    QByteArray turnPoint() const override;
private:
    int     m_camerId = 0x01;
    quint16 m_ww  = 0x1F;
    quint16 m_vv  = 0x1F;
};

class PELCODProtocal final : public protocalBase
{
    Q_OBJECT
public:
    PELCODProtocal(QObject *parent = nullptr)
        : protocalBase(parent) {}
    ~PELCODProtocal() override;

    QString name()         const override;
    QByteArray swit()      const override;
    int  camerId(const int &camerId)    override;
    quint16    vv(const quint16& vv)    override;
    quint16    ww(const quint16&  ww)   override;
    QByteArray up()        const override;
    QByteArray down()      const override;
    QByteArray left()      const override;
    QByteArray right()     const override;
    QByteArray bigger()    const override;
    QByteArray smaller()   const override;
    QByteArray stop()      const override;
    QByteArray clearPoint()const override;
    QByteArray setPoint()  const override;
    QByteArray turnPoint() const override;
private:
    int m_camerId = 0x01;
    quint16 m_ww  = 0x1F;
    quint16 m_vv  = 0x1F;
};


class PELCOPProtocal final : public protocalBase
{
    Q_OBJECT
public:
    PELCOPProtocal( QObject *parent = nullptr)
        : protocalBase(parent) {}
    ~PELCOPProtocal() override;

    QString name()         const override;
    QByteArray swit()      const override;
    int  camerId(const int &camerId)    override;
    quint16    vv(const quint16& vv)    override;
    quint16    ww(const quint16&  ww)   override;
    QByteArray up()        const override;
    QByteArray down()      const override;
    QByteArray left()      const override;
    QByteArray right()     const override;
    QByteArray bigger()    const override;
    QByteArray smaller()   const override;
    QByteArray stop()      const override;
    QByteArray clearPoint()const override;
    QByteArray setPoint()  const override;
    QByteArray turnPoint() const override;
private:
    int m_camerId = 0x01;
    quint16 m_ww  = 0x1F;
    quint16 m_vv  = 0x1F;
};
