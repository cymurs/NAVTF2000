#ifndef _DEFINEFILE_H_
#define _DEFINEFILE_H_

#include <vector>

// --------------------------------------------------------------------------------------
// Define file
// --------------------------------------------------------------------------------------

// 释放指针宏
#define RELEASE_ONE(x)                  {if(x != NULL ){delete x;x=NULL;}}

#define RELEASE_MORE(x)                 {if(x != NULL ){delete[] x;x=NULL;}}

// 释放句柄宏
#define RELEASE_HANDLE(x)               {if(x != NULL && x!=INVALID_HANDLE_VALUE){ CloseHandle(x);x = NULL;}}

// 释放Socket宏
#define RELEASE_SOCKET(x)               {if(x !=INVALID_SOCKET) { closesocket(x);x=INVALID_SOCKET;}}

// 系统使用的配置文件
#define SYS_INI_FILE_NAME               ".\\system.ini"

// 用户使用的配置文件
#define USER_INI_FILE_NAME              ".\\user.ini"

// Com 缓冲区 大小
#define MAX_SIZE_OF_SAVE_BUF        (1024*8)
#define MAX_SIZE_OF_TEMP_BUF        (1024*8)
#define MAX_SIZE_OF_RECV_BUF        (1024*1)
#define MAX_SIZE_OF_SEND_BUF        (1024*1)

#define SIZE_OF_INPUT_BUF           (1024*4)

// --------------------------------------------------------------------------------------
// 命令字
#define COMMAND_IS_AT               0x01        // AT 命令
#define COMMAND_IS_XXB2             0x02        // XXB2 接收机 命令

// --------------------------------------------------------------------------------------
// Navtf 功能管理：某些页或者某些页内的某些功能，可以选择是否使用
#define PAGE_INDEX_OF_UPDATE        0x00        // 通信/升级 页 --------------------------
#define PAGE_UPDATE_ENTER_CHECK     0x0001                  // “回车” 选项
//#define PAGE_UPDATE_ALONE_OR_SYSTEM 0x0002                  // “单板”“系统”选项：如选择单板，则可以使用独立单元，但没有自动升级
#define PAGE_UPDATE_REF_STATUS      0x0003                  // 参考输入状态显示，获取设备状态

#define PAGE_UPDATE_EXPORT_FILE     0x0080                  // (大于80为Admin功能) 导出文件功能

#define PAGE_INDEX_OF_SIGNAL        0x01        // 信号选择 页 ---------------------------
#define PAGE_SIGNAL_INPUT_CTRL      0x0101                  // 全部的数值修改和显示
#define PAGE_SIGNAL_PRIORITY        0x0102                  // 优先级设置和显示

#define PAGE_INDEX_OF_STATUS        0x02        // 状态监控 页 ---------------------------
#define PAGE_INDEX_OF_ADJUST        0x03        // 设备调整 页 ---------------------------
#define PAGE_INDEX_OF_GNSS          0x04        // GNSS 页 -------------------------------


struct st_PageItemData
{
    st_PageItemData()
    {
        m_iPageItemIndex = 0;
        m_bPageItemUse = false;
    }

    int m_iPageItemIndex;                       // 功能项序号
    bool m_bPageItemUse;                        // 是否使用
};

struct st_PageData
{
    st_PageData()
    {
        m_iPageIndex = 0;
        m_bPageUse = false;
        m_iPageItemNums = 0;
    }

    void Clear()
    {
        m_PageItem_vector.clear();
        m_iPageItemNums = 0;
    }

    void AddOne(st_PageItemData& oPD)
    {
        m_PageItem_vector.push_back(oPD);
        ++m_iPageItemNums;
    }

    int GetPageItemNums()
    {
        return m_iPageItemNums;
    }

    int m_iPageIndex;                                   // 页序号
    bool m_bPageUse;                                    // 该页是否使用
    std::vector<st_PageItemData> m_PageItem_vector;     // 该页要检测的功能项

private:
    int m_iPageItemNums;                                // 该页内的功能项的检测的数量，0=不检测，全使用
};

struct st_PageCheck
{
    st_PageCheck()
    {
        m_bAdmin = false;
        m_iPageNums = 0;
    }

    void Clear()
    {
        m_PageData_vector.clear();
        m_iPageNums = 0;
    }

    void AddOne(st_PageData& oPageData)
    {
        m_PageData_vector.push_back(oPageData);
        ++m_iPageNums;
    }

    int GetPageNums()
    {
        return m_iPageNums;
    }

    bool IsAdmin()
    {
        return m_bAdmin;
    }

    // Update Tab
    bool UpdateTabVisiable()
    {
        return PageTabIsVisiable(PAGE_INDEX_OF_UPDATE);
    }
    bool UpdateTab_EnterCheck_Use()
    {
        return PageTabItemCanUse(PAGE_INDEX_OF_UPDATE, PAGE_UPDATE_ENTER_CHECK);
    }
    //bool UpdateTab_AloneOrSys_Use()
    //{
    //    return PageTabItemCanUse(PAGE_INDEX_OF_UPDATE, PAGE_UPDATE_ALONE_OR_SYSTEM);
    //}
    bool UpdateTab_RefStatus_Use()
    {
        return PageTabItemCanUse(PAGE_INDEX_OF_UPDATE, PAGE_UPDATE_REF_STATUS);
    }
    bool UpdateTab_ExportFile_Use()
    {
        return PageTabItemCanUse(PAGE_INDEX_OF_UPDATE, PAGE_UPDATE_EXPORT_FILE);
    }

    // Signal Tab
    bool SignalTabVisiable()
    {
        return PageTabIsVisiable(PAGE_INDEX_OF_SIGNAL);
    }
    bool SignalTab_InputCtrl_Use()
    {
        return PageTabItemCanUse(PAGE_INDEX_OF_SIGNAL, PAGE_SIGNAL_INPUT_CTRL);
    }
    bool SignalTab_Priority_Use()
    {
        return PageTabItemCanUse(PAGE_INDEX_OF_SIGNAL, PAGE_SIGNAL_PRIORITY);
    }

    // Status Tab
    bool StatusTabVisiable()
    {
        return PageTabIsVisiable(PAGE_INDEX_OF_STATUS);
    }

    // Adjust Tab
    bool AdjustTabVisiable()
    {
        return PageTabIsVisiable(PAGE_INDEX_OF_ADJUST);
    }

    // Gnss Tab
    bool GnssTabVisiable()
    {
        return PageTabIsVisiable(PAGE_INDEX_OF_GNSS);
    }

private:
    // 某页是否可见
    bool PageTabIsVisiable(int iPageIndex)
    {
        bool bVisiable = false;
        int iCount = qMin(m_iPageNums, (int)m_PageData_vector.size());
        for(int i = 0; i < iCount; ++i)
        {
            st_PageData& oPD = m_PageData_vector[i];
            if((iPageIndex == oPD.m_iPageIndex) && (true == oPD.m_bPageUse))
            {
                bVisiable = true;
                break;
            }
        }
        return bVisiable;
    }

    // 某页某项是否可用
    bool PageTabItemCanUse(int iPageIndex, int iPageItem)
    {
        bool bCanUse = false;
        int iCount = qMin(m_iPageNums, (int)m_PageData_vector.size());
        for(int i = 0; i < iCount; ++i)
        {
            st_PageData& oPD = m_PageData_vector[i];
            if((iPageIndex == oPD.m_iPageIndex) && (true == oPD.m_bPageUse))
            {
                int iNum = qMin(oPD.GetPageItemNums(), (int)oPD.m_PageItem_vector.size());
                for(int j = 0; j < iNum; ++j)
                {
                    st_PageItemData& oPID = oPD.m_PageItem_vector[j];
                    if((iPageItem == oPID.m_iPageItemIndex) && (true == oPID.m_bPageItemUse))
                    {
                        bCanUse = true;
                        break;
                    }
                }
                break;
            }
        }
        return bCanUse;
    }

public:
    bool m_bAdmin;                                      // 管理员
    std::vector<st_PageData> m_PageData_vector;         // 页内容

private:
    int m_iPageNums;                                    // 多少页
};


#endif
