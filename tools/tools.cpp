#include "tools.h"
#include <QUrl>
#include <QDir>

Tools::Tools() {}

QString Tools::URIconvertFile(QString path)
{
    QUrl url(path);
    QString localPath;
    if (url.isLocalFile()) {
        localPath = url.toLocalFile(); // 自动解码 %5B 变成 [，并去掉 file:///
    } else {
        // 如果传进来的是普通字符串但带百分号，手动解码
        localPath = QUrl::fromPercentEncoding(path.toUtf8());
    }
    return localPath;
}
