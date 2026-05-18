#pragma once

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
#ifndef NOMINMAX
#define NOMINMAX                        // 避免 min/max 宏与 std::min 等冲突（含第三方头时常见）
#endif
// Windows 头文件
#include <windows.h>
