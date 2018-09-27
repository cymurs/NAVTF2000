#ifndef _DEFINEFILE_H_
#define _DEFINEFILE_H_

#include <vector>

// --------------------------------------------------------------------------------------
// Define file
// --------------------------------------------------------------------------------------

// �ͷ�ָ���
#define RELEASE_ONE(x)                  {if(x != NULL ){delete x;x=NULL;}}

#define RELEASE_MORE(x)                 {if(x != NULL ){delete[] x;x=NULL;}}

// �ͷž����
#define RELEASE_HANDLE(x)               {if(x != NULL && x!=INVALID_HANDLE_VALUE){ CloseHandle(x);x = NULL;}}

// �ͷ�Socket��
#define RELEASE_SOCKET(x)               {if(x !=INVALID_SOCKET) { closesocket(x);x=INVALID_SOCKET;}}

// ϵͳʹ�õ������ļ�
#define SYS_INI_FILE_NAME               ".\\system.ini"

// �û�ʹ�õ������ļ�
#define USER_INI_FILE_NAME              ".\\user.ini"

// Com ������ ��С
#define MAX_SIZE_OF_SAVE_BUF        (1024*8)
#define MAX_SIZE_OF_TEMP_BUF        (1024*8)
#define MAX_SIZE_OF_RECV_BUF        (1024*1)
#define MAX_SIZE_OF_SEND_BUF        (1024*1)

#define SIZE_OF_INPUT_BUF           (1024*4)

// --------------------------------------------------------------------------------------
// ������
#define COMMAND_IS_AT               0x01        // AT ����
#define COMMAND_IS_XXB2             0x02        // XXB2 ���ջ� ����

// --------------------------------------------------------------------------------------
// Navtf ���ܹ���ĳЩҳ����ĳЩҳ�ڵ�ĳЩ���ܣ�����ѡ���Ƿ�ʹ��
#define PAGE_INDEX_OF_UPDATE        0x00        // ͨ��/���� ҳ --------------------------
#define PAGE_UPDATE_ENTER_CHECK     0x0001                  // ���س��� ѡ��
//#define PAGE_UPDATE_ALONE_OR_SYSTEM 0x0002                  // �����塱��ϵͳ��ѡ���ѡ�񵥰壬�����ʹ�ö�����Ԫ����û���Զ�����
#define PAGE_UPDATE_REF_STATUS      0x0003                  // �ο�����״̬��ʾ����ȡ�豸״̬

#define PAGE_UPDATE_EXPORT_FILE     0x0080                  // (����80ΪAdmin����) �����ļ�����

#define PAGE_INDEX_OF_SIGNAL        0x01        // �ź�ѡ�� ҳ ---------------------------
#define PAGE_SIGNAL_INPUT_CTRL      0x0101                  // ȫ������ֵ�޸ĺ���ʾ
#define PAGE_SIGNAL_PRIORITY        0x0102                  // ���ȼ����ú���ʾ

#define PAGE_INDEX_OF_STATUS        0x02        // ״̬��� ҳ ---------------------------
#define PAGE_INDEX_OF_ADJUST        0x03        // �豸���� ҳ ---------------------------
#define PAGE_INDEX_OF_GNSS          0x04        // GNSS ҳ -------------------------------


struct st_PageItemData
{
    st_PageItemData()
    {
        m_iPageItemIndex = 0;
        m_bPageItemUse = false;
    }

    int m_iPageItemIndex;                       // ���������
    bool m_bPageItemUse;                        // �Ƿ�ʹ��
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

    int m_iPageIndex;                                   // ҳ���
    bool m_bPageUse;                                    // ��ҳ�Ƿ�ʹ��
    std::vector<st_PageItemData> m_PageItem_vector;     // ��ҳҪ���Ĺ�����

private:
    int m_iPageItemNums;                                // ��ҳ�ڵĹ�����ļ���������0=����⣬ȫʹ��
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
    // ĳҳ�Ƿ�ɼ�
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

    // ĳҳĳ���Ƿ����
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
    bool m_bAdmin;                                      // ����Ա
    std::vector<st_PageData> m_PageData_vector;         // ҳ����

private:
    int m_iPageNums;                                    // ����ҳ
};


#endif
