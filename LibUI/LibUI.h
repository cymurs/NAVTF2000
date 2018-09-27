#ifndef _LIBUI_H_
#define _LIBUI_H_

#include <windows.h>

/****************************************************************************************
    动态库头文件
*****************************************************************************************/
class QApplication;
class QWidget;
class QRect;

#ifdef __cplusplus
extern "C" {
#endif

    // 关闭窗口
    _declspec(dllexport) bool CloseQtWidgetAsUse();

    // 一、在 QT 环境下使用 -------------------------------------------------------------------------------------
    // 显示窗口：parent=0 表示作为主窗口使用，否则是子窗口
    _declspec(dllexport) void ShowQtWidgetAsQt(QApplication& myApp, QWidget* parent = 0);

    // 子窗口：移动
    _declspec(dllexport) void ResizeQtWidgetAsQt(QRect&);

    // 二、在 Win32 环境下使用 ----------------------------------------------------------------------------------
    // 显示窗口
    _declspec(dllexport) void ShowQtWidgetAsWin(HWND parent);

    // 移动窗口
    _declspec(dllexport) void MoveQtWidgetAsWin(int left, int top, int right, int bottom);


#ifdef __cplusplus
}
#endif


#endif
