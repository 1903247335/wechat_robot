#include "searcher.h"
Searcher::Searcher(QObject *parent): QObject(parent){
    m_timer = new QTimer(this);
connect(m_timer, &QTimer::timeout, this, &Searcher::searchWechat);

}
void Searcher::startTimer()
{
    m_timer->start(2000);

}

void Searcher::stopTimer()
{
    m_timer->stop();

}

void Searcher::searchWechat(){
    DWORD pid = 0;
    QString processName="WeChat.exe";
    HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return ;
    }
    PROCESSENTRY32W pe; // 使用 W 版本以支持 Unicode (Qt 默认)
    pe.dwSize = sizeof(PROCESSENTRY32W);
    if (Process32FirstW(hSnapshot, &pe)) {
        do {
            // 将遍历到的进程名转为 QString 进行忽略大小写的比较
            QString currentProcessName = QString::fromWCharArray(pe.szExeFile);

            if (currentProcessName.compare(processName, Qt::CaseInsensitive) == 0) {
                pid = pe.th32ProcessID;
                break; // 找到第一个匹配的就跳出循环
            }
        } while (Process32NextW(hSnapshot, &pe));
    }

    // 3. 清理句柄
    CloseHandle(hSnapshot);

    emit pidChanged(pid);

}
