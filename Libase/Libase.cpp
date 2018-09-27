#include "Libase.h"

CMyComBasic::CMyComBasic()
{

}
CMyComBasic::~CMyComBasic()
{

}

// 获取版本号，年月日
// 2015-06-26: "错误：CRC 不一致" 错误提示信息前面增加 \n
void CMyComBasic::GetDllLibVerAndDate(int& iLargeVer, int& iSmallVer, int& iYear, int& iMonth, int& iDay)
{
    iLargeVer = 1;
    iSmallVer = 5;

    iYear = 2015;
    iMonth = 8;
    iDay = 7;
}


