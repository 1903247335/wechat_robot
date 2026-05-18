#ifndef LOGGING_H

#define LOGGING_H

#include <QLoggingCategory>

// 定义一个名为 "network" 的分类
// 参数1：在代码中使用的变量名
// 参数2：在配置文件/日志中显示的字符串标识
Q_DECLARE_LOGGING_CATEGORY(logServer)
Q_DECLARE_LOGGING_CATEGORY(logModules)

#endif // LOGGING_H
