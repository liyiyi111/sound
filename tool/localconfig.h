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
// static QDir config(const QString& subdir = {});

// //加载本地
// static QString loadConfig(const QString& fileName, QJsonObject &obj, bool createIfNotExist = true);
// static QString loadConfig(const QString& fileName, QJsonArray &obj, bool createIfNotExist = true);
// static QString loadConfig(const QString& fileName, QJsonArray &arrs);


// //保存本地
// static QString saveConfig(const QString& fileName, QJsonObject &obj, bool createIfNotExist = true);
// static QString saveConfig(const QString& fileName, QJsonArray &arr, bool createIfNotExist = true);

}

namespace EasyBuf
{
//Op是一个模板模板参数，他期望一个模板，该模板接受一个类型参数
//Detector 检查着
template <typename, template<typename> typename, typename = void>
struct Detector : std::false_type { using type = void; };
template <typename T, template<typename> typename Op>
struct Detector<T, Op, std::void_t<Op<T>>> : std::true_type
{
    using type = typename Op<T>::type;
};

template<typename T>
struct  F
{
    using type = T;
    int length = 11;

};
}
}
