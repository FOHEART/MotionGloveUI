/**
 * @file  libh2ui_global.h
 * @brief 库导出/导入与静态库宏定义。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 纯声明或转发头，无运行期逻辑。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */

#pragma once

#include <QtCore/qglobal.h>

/**
 * @brief 库内所有导出类的可见性修饰宏。
 * @details 由 CMake 决定其取值：
 *          - 定义 `LIBH2UI_EXPORTS` 且为 Windows 时为 `__declspec(dllexport)`；
 *          - 定义 `LIBH2UI_STATIC_LIB` 时为空（静态链接不需要修饰）；
 *          - 其余 Windows 场景为 `__declspec(dllimport)`；
 *          - 非 Windows 平台一律为空。
 * @note 使用方不要自行定义本宏，交给构建脚本控制。
 */
#ifndef LIBH2UI_API
  #ifdef LIBH2UI_EXPORTS

    #ifdef _WIN32
    #define LIBH2UI_API __declspec(dllexport)
    #else
    #define LIBH2UI_API 
    #endif

  #else	

    #ifdef LIBH2UI_STATIC_LIB
      #define LIBH2UI_API
    #elif defined(_WIN32)
      #define LIBH2UI_API __declspec(dllimport)
    #else
      #define LIBH2UI_API
    #endif

  #endif	

#endif