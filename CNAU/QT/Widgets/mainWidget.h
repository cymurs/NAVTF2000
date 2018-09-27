#ifndef _MAINWIDGET_H_
#define _MAINWIDGET_H_

#include <QWidget>

#include "../../../Common/Define/DefineFile.h"
#include "../../../Common/Function/Function.h"

// --------------------------------------------------------------------------------------
// main widget
// --------------------------------------------------------------------------------------
class QGroupBox;
class QListWidget;
class QPushButton;
class QLineEdit;
class QCheckBox;
class QListWidgetItem;
class QLabel;

// --------------------------------------------------------------------------------------
class CMainWidget : public QWidget
{
    Q_OBJECT
public:
    CMainWidget(QWidget* parent = 0);
    ~CMainWidget();

public:

private:
    void CreateWidgets();

    QGroupBox* CreateOptionGroup();
    QGroupBox* CreateFileGroup();

private:
    void InitPageCheckData();
    void FillPageCheckInitToCtrl();

    int GetPageOrItemIndex(QString& strIndexName);

    void ShowPageUse();
    void ShowPageItemUse(int iPageIndex);

private:
    QListWidget* m_pPage_ListWidget;
    QPushButton* m_pPageAdd_Button;
    QListWidget* m_pPageUse_ListWidget;
    QPushButton* m_pPageUseClear_Button;

    QListWidget* m_pPageItem_ListWidget;
    QPushButton* m_pPageItemAdd_Button;
    QListWidget* m_pPageItemUse_ListWidget;
    QPushButton* m_pPageItemUseClear_Button;

    QLineEdit* m_pFileName_Edit;
    QLineEdit* m_pFileVer_Edit;
    QCheckBox* m_pAdmin_CheckBox;

    QPushButton* m_pFileOpen_Button;
    QLineEdit* m_pFileOpenPath_Edit;
    QPushButton* m_pFileOpenRead_Button;

    QPushButton* m_pPathSelect_Button;
    QLineEdit* m_pPathSelect_Edit;
    QPushButton* m_pPathFileSave_Button;

    QLabel* m_pInfoShow_Label;

private slots:
    void SelectOneFile_Slot();
    void SelectOnePath_Slot();

    void PageClicked_Slot(QListWidgetItem*);
    void PageUseClicked_Slot(QListWidgetItem*);
    void PageItemClicked_Slot(QListWidgetItem*);

    void PageAdd_Slot();
    void PageItemAdd_Slot();

    void PageUseClear_Slot();
    void PageItemUseClear_Slot();

    void AdminCheck_Slot();

    void FileOpenRead_Slot();
    void PathFileSave_Slot();

private:
    st_PageCheck m_oPageCheck_Init;             // 用于初始化
    st_PageCheck m_oPageCheck_Use;              // 使用

};




#endif
