#ifndef SEARCHER_H
#define SEARCHER_H
#include"global.h"
#include <tlhelp32.h>
class Searcher:public QObject
{
    Q_OBJECT

public:
    QTimer *m_timer;
public:
    explicit Searcher(QObject *parent = nullptr);
    Q_INVOKABLE void startTimer();
    Q_INVOKABLE void stopTimer();
    void searchWechat();
signals:
    // 当 PID 变化时发射此信号
    void pidChanged(int newPid);

};

#endif // SEARCHER_H
