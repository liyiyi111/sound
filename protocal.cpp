#include "protocal.h"

VISCAProtocal::~VISCAProtocal()
{

}

QString VISCAProtocal::name() const
{
    return u"VISCA"_qs;
}

QByteArray VISCAProtocal::swit() const
{
    return {};
}

int VISCAProtocal::camerId(const int& camerId)
{
    m_camerId = camerId;
    return m_camerId;
}

quint16 VISCAProtocal::vv(const quint16& vv)
{
    m_vv = vv;
    return m_vv;
}

quint16 VISCAProtocal::ww(const quint16& ww)
{
    m_ww = ww;
    return m_ww;
}

QByteArray VISCAProtocal::up() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x06);
    bytes.append(0x01);
    bytes.append(0x1F);
    bytes.append(0x1F);
    bytes.append(0x03);
    bytes.append(0x01);
    bytes.append(0xFF);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray VISCAProtocal::down() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x06);
    bytes.append(0x01);
    bytes.append(0x1F);
    bytes.append(0x1F);
    bytes.append(0x03);
    bytes.append(0x02);
    bytes.append(0xFF);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray VISCAProtocal::left() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x06);
    bytes.append(0x01);
    bytes.append(0x1F);
    bytes.append(0x1F);
    bytes.append(0x01);
    bytes.append(0x03);
    bytes.append(0xFF);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray VISCAProtocal::right() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x06);
    bytes.append(0x01);
    bytes.append(0x1F);
    bytes.append(0x1F);
    bytes.append(0x02);
    bytes.append(0x03);
    bytes.append(0xFF);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray VISCAProtocal::bigger() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x04);
    bytes.append(0x07);
    bytes.append(0x02);
    bytes.append(0xFF);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray VISCAProtocal::smaller() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x04);
    bytes.append(0x07);
    bytes.append(0x03);
    bytes.append(0xFF);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray VISCAProtocal::stop() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x04);
    bytes.append(0x07);
    bytes.append((char)0x00);
    bytes.append(0xFF);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray VISCAProtocal::clearPoint() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x04);
    bytes.append(0x3F);
    bytes.append((char)0x00);
    bytes.append(0x1F);
    bytes.append(0xFF);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray VISCAProtocal::setPoint() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x04);
    bytes.append(0x3F);
    bytes.append(0x02);
    bytes.append(0x1F);
    bytes.append(0xFF);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray VISCAProtocal::turnPoint() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x04);
    bytes.append(0x3F);
    bytes.append(0x01);
    bytes.append(0x1F);
    bytes.append(0xFF);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

protocalController::protocalController(QObject *parent) :
    QObject{parent}
{
    printTime([&] {initStandard();}, "protocalController", __LINE__);
}

protocalController::~protocalController()
{

}

protocalBase *protocalController::base(const QString &protocalName) const
{
    auto pos = m_base.find(protocalName);
    if(pos != m_base.end())
        return pos.value();
}

void protocalController::initStandard()
{
    using protocalPanels = Items<
        VISCAProtocal,
        PELCODProtocal,
        PELCOPProtocal
        >;

    protocalPanels panels;
    panels.forEach([this](protocalBase *base) {
        //std::apply([&protocalBase base](auto&&... items) {(f(items), ...);}, this->items);
        m_base.insert(base->name(), base);
    });
}

PELCODProtocal::~PELCODProtocal()
{

}

QString PELCODProtocal::name() const
{
    return u"PELCO-D"_qs;
}

QByteArray PELCODProtocal::swit() const
{
    return {};
}

int PELCODProtocal::camerId(const int& camerId)
{
    m_camerId = camerId;
    return m_camerId;
}

quint16 PELCODProtocal::vv(const quint16& vv)
{
    m_vv = vv;
    return m_vv;
}

quint16 PELCODProtocal::ww(const quint16& ww)
{
    m_ww = ww;
    return m_ww;
}


QByteArray PELCODProtocal::up() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x06);
    bytes.append(0x01);
    bytes.append(0x1F);
    bytes.append(0x1F);
    bytes.append(0x03);
    bytes.append(0x01);
    bytes.append(0xFF);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray PELCODProtocal::down() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x06);
    bytes.append(0x01);
    bytes.append(0x1F);
    bytes.append(0x1F);
    bytes.append(0x03);
    bytes.append(0x02);
    bytes.append(0xFF);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray PELCODProtocal::left() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x06);
    bytes.append(0x01);
    bytes.append(0x1F);
    bytes.append(0x1F);
    bytes.append(0x01);
    bytes.append(0x03);
    bytes.append(0xFF);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray PELCODProtocal::right() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x06);
    bytes.append(0x01);
    bytes.append(0x1F);
    bytes.append(0x1F);
    bytes.append(0x02);
    bytes.append(0x03);
    bytes.append(0xFF);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray PELCODProtocal::bigger() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x04);
    bytes.append(0x07);
    bytes.append(0x02);
    bytes.append(0xFF);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray PELCODProtocal::smaller() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x04);
    bytes.append(0x07);
    bytes.append(0x03);
    bytes.append(0xFF);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray PELCODProtocal::stop() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x04);
    bytes.append(0x07);
    bytes.append((char)0x00);
    bytes.append(0xFF);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray PELCODProtocal::clearPoint() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x04);
    bytes.append(0x3F);
    bytes.append((char)0x00);
    bytes.append(0x1F);
    bytes.append(0xFF);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray PELCODProtocal::setPoint() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x04);
    bytes.append(0x3F);
    bytes.append(0x02);
    bytes.append(0x1F);
    bytes.append(0xFF);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray PELCODProtocal::turnPoint() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x04);
    bytes.append(0x3F);
    bytes.append(0x01);
    bytes.append(0x1F);
    bytes.append(0xFF);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

PELCOPProtocal::~PELCOPProtocal()
{

}

QString PELCOPProtocal::name() const
{
    return u"PELCO-P"_qs;
}

QByteArray PELCOPProtocal::swit() const
{
    return {};
}

int PELCOPProtocal::camerId(const int& camerId)
{
    m_camerId = camerId;
    return m_camerId;
}

quint16 PELCOPProtocal::vv(const quint16& vv)
{
    m_vv = vv;
    return m_vv;
}

quint16 PELCOPProtocal::ww(const quint16& ww)
{
    m_ww = ww;
    return m_ww;
}

QByteArray PELCOPProtocal::up() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x06);
    bytes.append(0x01);
    bytes.append(0x1F);
    bytes.append(0x1F);
    bytes.append(0x03);
    bytes.append(0x01);
    bytes.append(0xFF);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray PELCOPProtocal::down() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x06);
    bytes.append(0x01);
    bytes.append(0x1F);
    bytes.append(0x1F);
    bytes.append(0x03);
    bytes.append(0x02);
    bytes.append(0xFF);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray PELCOPProtocal::left() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x06);
    bytes.append(0x01);
    bytes.append(0x1F);
    bytes.append(0x1F);
    bytes.append(0x01);
    bytes.append(0x03);
    bytes.append(0xFF);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray PELCOPProtocal::right() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x06);
    bytes.append(0x01);
    bytes.append(0x1F);
    bytes.append(0x1F);
    bytes.append(0x02);
    bytes.append(0x03);
    bytes.append(0xFF);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray PELCOPProtocal::bigger() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x04);
    bytes.append(0x07);
    bytes.append(0x02);
    bytes.append(0xFF);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray PELCOPProtocal::smaller() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x04);
    bytes.append(0x07);
    bytes.append(0x03);
    bytes.append(0xFF);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray PELCOPProtocal::stop() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x04);
    bytes.append(0x07);
    bytes.append((char)0x00);
    bytes.append(0xFF);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray PELCOPProtocal::clearPoint() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x04);
    bytes.append(0x3F);
    bytes.append((char)0x00);
    bytes.append(0x1F);
    bytes.append(0xFF);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray PELCOPProtocal::setPoint() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x04);
    bytes.append(0x3F);
    bytes.append(0x02);
    bytes.append(0x1F);
    bytes.append(0xFF);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}

QByteArray PELCOPProtocal::turnPoint() const
{
    QByteArray bytes;
    bytes.append(0xFC);
    bytes.append(0x80 | (m_camerId & 0xFF));
    bytes.append(0x01);
    bytes.append(0x04);
    bytes.append(0x3F);
    bytes.append(0x01);
    bytes.append(0x1F);
    bytes.append(0xFF);
    bytes.append((char)0x00);
    bytes.append((char)0x00);
    bytes.append(0xFD);
    assert(bytes.size() == 11);
    return bytes;
}
