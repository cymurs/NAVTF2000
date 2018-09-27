#ifndef _LIBUI_H_
#define _LIBUI_H_

#include <windows.h>

/****************************************************************************************
    ��̬��ͷ�ļ�
*****************************************************************************************/
class QApplication;
class QWidget;
class QRect;

#ifdef __cplusplus
extern "C" {
#endif

    // �رմ���
    _declspec(dllexport) bool CloseQtWidgetAsUse();

    // һ���� QT ������ʹ�� -------------------------------------------------------------------------------------
    // ��ʾ���ڣ�parent=0 ��ʾ��Ϊ������ʹ�ã��������Ӵ���
    _declspec(dllexport) void ShowQtWidgetAsQt(QApplication& myApp, QWidget* parent = 0);

    // �Ӵ��ڣ��ƶ�
    _declspec(dllexport) void ResizeQtWidgetAsQt(QRect&);

    // ������ Win32 ������ʹ�� ----------------------------------------------------------------------------------
    // ��ʾ����
    _declspec(dllexport) void ShowQtWidgetAsWin(HWND parent);

    // �ƶ�����
    _declspec(dllexport) void MoveQtWidgetAsWin(int left, int top, int right, int bottom);


#ifdef __cplusplus
}
#endif


#endif
