#ifndef WECHATMANAGER_H
#define WECHATMANAGER_H

#include <QObject>

class WechatManager:public QObject
{
    Q_OBJECT
public:
    WechatManager();
    Q_INVOKABLE void sendMessage(const QString &name, const QString &content,const QString &type);
};

#endif // WECHATMANAGER_H
