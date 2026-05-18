#ifndef STARTER_H
#define STARTER_H
#include"global.h"

class Starter:public QObject
{
    Q_OBJECT

public:
    Starter();
    Q_INVOKABLE bool startWeChat(const QString& path,const QString&dllPath);
};

#endif // STARTER_H
