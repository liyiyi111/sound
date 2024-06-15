#include "localconfig.h"

namespace Tools
{
namespace  LocalConfig
{
// static QString readFile(const QString &fileName, QByteArray &content, bool createIfnotExist)
// {
//     QFile f;
//     if(fileName.startsWith(":") || fileName.startsWith("qrc:"))
//     {
//         f.setFileName(fileName);
//         if(!f.open(QFile::ReadOnly)) return f.errorString();
//     }
//     return {};
// }

// QString loadConfig(const QString &fileName, QJsonObject &obj, bool createIfNotExist)
// {
//     QFile f(fileName);
//     QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
//     return {};
// }

// QString loadConfig(const QString &fileName, QJsonArray &obj, bool createIfNotExist)
// {
//     return {};
// }

// QString saveConfig(const QString &fileName, QJsonObject &obj, bool createIfNotExist)
// {
//     return {};
// }

// QString saveConfig(const QString &fileName, QJsonArray &arr, bool createIfNotExist)
// {
//     return {};
// }

// QDir config(const QString &subdir)
// {
//     auto dirs = QStandardPaths::StandardLocation(QStandardPaths::GenericConfigLocation);
//     return {};
// }

// QString loadConfig(const QString &fileName, QJsonArray &arrs)
// {
//     QFile f(fileName);
//     if(!f.open(QFile::ReadOnly)) return f.errorString();

//     QJsonParseError error;
//     auto doc = QJsonDocument::fromJson(f.readAll(), &error);
//     arrs = doc.array();
//     if(arrs.isEmpty())
//         return error.errorString();
//     return {};
// }
// }

}

}
