#ifndef LOCALCONFIG_H
#define LOCALCONFIG_H

#include <QObject>

class localConfig : public QObject
{
    Q_OBJECT
public:
    explicit localConfig(QObject *parent = nullptr);

signals:
};

#endif // LOCALCONFIG_H
