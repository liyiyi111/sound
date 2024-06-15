#pragma once
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QFileDialog>
#include <QStandardPaths>

namespace Tools
{

namespace LocalConfig
{
static QDir config(const QString& subdir = {});

//加载本地
static QString loadConfig(const QString& fileName, QJsonObject &obj, bool createIfNotExist = true);

static QString loadConfig(const QString& fileName, QJsonArray &obj, bool createIfNotExist = true);

//
static QString loadConfig(const QString& fileName, QJsonArray &arrs);


//保存本地
static QString saveConfig(const QString& fileName, QJsonObject &obj, bool createIfNotExist = true);
static QString saveConfig(const QString& fileName, QJsonArray &arr, bool createIfNotExist = true);

}
}
