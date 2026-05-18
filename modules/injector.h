#ifndef INJECTOR_H
#define INJECTOR_H
#include"global.h"

class Injector: public QObject
{
    Q_OBJECT
public:
    Injector();
    bool startInject(DWORD processId, const QString& dllPath);

};

#endif // INJECTOR_H
