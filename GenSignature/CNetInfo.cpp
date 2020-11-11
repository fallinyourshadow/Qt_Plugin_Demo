#include <atlbase.h>
#include <atlconv.h>
#include "CNetInfo.h"
#include "iphlpapi.h"
#include "string.h"
#include <QDebug>
//************************************************************************/
//* Fuction:        CNetInfo()
//* Return:
//* PrameterList:
//* Description:
//************************************************************************/
CNetInfo::CNetInfo(QObject *parent) : QObject(parent)
{
    m_totalNetCards = 0;
    _tcscpy(m_szDomain,_T(""));
    _tcscpy(m_szHostName,_T(""));
    m_IPEnableRouter = 2;
    m_enableDNS = 2;
    m_systemType = Unknow;
}
//***********************************************************************/
///* Fuction:        ~CNetInfo()
///* Return:
///* PrameterList:
///* Description:
///***********************************************************************
CNetInfo::~CNetInfo()
{

}

int CNetInfo::totalNetCards()
{
    return m_totalNetCards;
}

CNetInfo::NET_CARD CNetInfo::Cards(qint32 num)
{
    return m_cards[num];
}

CNetInfo::MAC_ADDRESS CNetInfo::macAddr(qint32 num)
{
    return  m_macAddr[num];
}

TCHAR * CNetInfo::szHostName()
{
    return m_szHostName;
}
//***********************************************************************/
///* Fuction:        GetNetInfo()
///* Return:
///* PrameterList:
///* Description:
///***********************************************************************
BOOL CNetInfo::GetNetInfo()
{
    m_systemType = GetSystemType();

    if (m_systemType == Windows9X)
    {
        return GetNetInfoOfWin9X();
    }
    else if(m_systemType == WinNT4orHigher)
    {
        return GetNetInfoOfWinNT();
    }
    else//不支持老旧的操作系统
    {
        return FALSE;
    }
}
//***********************************************************************/
///* Fuction:        GetSystemType()
///* Return:
///* PrameterList:
///* Description:
///***********************************************************************
CNetInfo::Win32Type CNetInfo::GetSystemType()
{
    Win32Type        SystemType;
    DWORD            winVer;
    OSVERSIONINFO*    osvi;

    winVer = GetVersion();
    if(winVer < 0x80000000)
    {
        /*NT */
        SystemType = WinNT3;
        osvi = (OSVERSIONINFO *)malloc(sizeof(OSVERSIONINFO));
        if (osvi != NULL)
        {
            memset(osvi,0,sizeof(OSVERSIONINFO));
            osvi->dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
            GetVersionEx(osvi);
            if (osvi->dwMajorVersion >= 4L)
            {
                SystemType = WinNT4orHigher;
            }
            free(osvi);
        }
    }
    else if  (LOBYTE(LOWORD(winVer)) < 4)
    {
        SystemType=Win32s;/*Win32s*/
    }
    else
    {
        SystemType=Windows9X;/*Windows9X*/
    }
    return SystemType;
}
//***********************************************************************/
///* Fuction:        GetNetInfoOfWin9X()
///* Return:
///* PrameterList:
///* Description:
///***********************************************************************
BOOL CNetInfo::GetNetInfoOfWin9X()
{
    LONG lRet;
    HKEY hMainKey;
    TCHAR szNameServer[256];

    //得到域名，网关和DNS的设置
    lRet = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("System\\CurrentControlSet\\Services\\VxD\\MSTCP"),0,KEY_READ,&hMainKey);
    if(lRet == ERROR_SUCCESS)
    {
        DWORD dwType,dwDataSize = 256;
        ::RegQueryValueEx(hMainKey,_T("Domain"),NULL,&dwType,(LPBYTE)m_szDomain,&dwDataSize);
        dwDataSize = 256;
        ::RegQueryValueEx(hMainKey,_T("Hostname"),NULL,&dwType,(LPBYTE)m_szHostName,&dwDataSize);
        dwDataSize = 256;
        ::RegQueryValueEx(hMainKey,_T("EnableDNS"),NULL,&dwType,(LPBYTE)&m_enableDNS,&dwDataSize);
        dwDataSize = 256;
        ::RegQueryValueEx(hMainKey,_T("NameServer"),NULL,&dwType,(LPBYTE)szNameServer,&dwDataSize);
    }
    ::RegCloseKey(hMainKey);

    HKEY hNetCard = NULL;
    //调用CTcpCfg类的静态函数得到网卡的数目和相应的MAC地址
    m_totalNetCards = GetMacAddress();

    lRet = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("System\\CurrentControlSet\\Services\\Class\\Net"),0,KEY_READ,&hNetCard);
    if(lRet != ERROR_SUCCESS)//此处失败就返回
    {
        if(hNetCard != NULL)
        {
            ::RegCloseKey(hNetCard);
        }
        return FALSE;
    }

    DWORD dwSubKeyNum = 0,dwSubKeyLen = 256;
    //得到子键的个数，通常与网卡个数相等
    lRet = ::RegQueryInfoKey(hNetCard,NULL,NULL,NULL,&dwSubKeyNum,&dwSubKeyLen,NULL,NULL,NULL,NULL,NULL,NULL);
    if(lRet == ERROR_SUCCESS)
    {
        //m_TotalNetCards = dwSubKeyNum;//网卡个数以此为主
        LPTSTR lpszKeyName = new TCHAR[dwSubKeyLen + 1];
        DWORD dwSize;
        for(int i = 0; i < (int)m_totalNetCards; i++)
        {
            TCHAR szNewKey[256];
            HKEY hNewKey;
            DWORD dwType = REG_SZ,dwDataSize = 256;
            dwSize = dwSubKeyLen + 1;
            lRet = ::RegEnumKeyEx(hNetCard,i,lpszKeyName,&dwSize,NULL,NULL,NULL,NULL);
            if(lRet == ERROR_SUCCESS)
            {
                lRet = ::RegOpenKeyEx(hNetCard,lpszKeyName,0,KEY_READ,&hNewKey);
                if(lRet == ERROR_SUCCESS)
                {
                    ::RegQueryValueEx(hNewKey,_T("DriverDesc"),NULL,&dwType,(LPBYTE)m_cards[i].szDescription,&dwDataSize);
                }
                ::RegCloseKey(hNewKey);
                wsprintf(szNewKey,_T("System\\CurrentControlSet\\Services\\Class\\NetTrans\\%s"),lpszKeyName);
                lRet = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,szNewKey,0,KEY_READ,&hNewKey);
                if(lRet == ERROR_SUCCESS)
                {
                    dwDataSize = 256;
                    ::RegQueryValueEx(hNewKey,_T("DefaultGateway"),NULL,&dwType,(LPBYTE)m_cards[i].szGateWay,&dwDataSize);
                    ProcessMultiString(m_cards[i].szGateWay,dwDataSize);
                    dwDataSize = 256;
                    ::RegQueryValueEx(hNewKey,_T("IPAddress"),NULL,&dwType,(LPBYTE)m_cards[i].szIpAddress,&dwDataSize);
                    ProcessMultiString(m_cards[i].szIpAddress,dwDataSize);
                    dwDataSize = 256;
                    ::RegQueryValueEx(hNewKey,_T("IPMask"),NULL,&dwType,(LPBYTE)m_cards[i].szIpMask,&dwDataSize);
                    ProcessMultiString(m_cards[i].szIpMask,dwDataSize);
                    //拷贝前面得到的DNS主机名
                    _tcscpy(m_cards[i].szDNSNameServer,szNameServer);
                }
                ::RegCloseKey(hNewKey);
            }
            //            m_cards[i].szMacAddr[0] = m_macAddr[i].b1;
            //            m_cards[i].szMacAddr[1] = m_macAddr[i].b2;
            //            m_cards[i].szMacAddr[2] = m_macAddr[i].b3;
            //            m_cards[i].szMacAddr[3] = m_macAddr[i].b4;
            //            m_cards[i].szMacAddr[4] = m_macAddr[i].b5;
            //            m_cards[i].szMacAddr[5] = m_macAddr[i].b6;
        }
    }
    ::RegCloseKey(hNetCard);
    return lRet == ERROR_SUCCESS ? TRUE : FALSE;
}
//***********************************************************************/
///* Fuction:        GetMacAddress()
///* Return:
///* PrameterList:
///* Description:
///***********************************************************************
int CNetInfo::GetMacAddress()
{
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;
    pAdapterInfo = (IP_ADAPTER_INFO* )malloc(sizeof(IP_ADAPTER_INFO));
    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    if(GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) != ERROR_SUCCESS)
    {
        free(pAdapterInfo);
        pAdapterInfo=(IP_ADAPTER_INFO*)malloc(ulOutBufLen);
    }
    int i = 0;
    if((dwRetVal=GetAdaptersInfo(pAdapterInfo,&ulOutBufLen))==NO_ERROR)
    {
        pAdapter=pAdapterInfo;
        while(pAdapter)
        {
            QString des(pAdapter->Description);
            //            if(
            //                des.contains("PCI")//pAdapter->Description中包含"PCI"为：物理网卡
            //                ||pAdapter->Type==71        //pAdapter->Type是71为：无线网卡
            //                )
            //            {
            qDebug() << "------------------------------------------------------------n";
            qDebug() << "AdapterName:" << QString(pAdapter->AdapterName);
            qDebug() << "AdapterDesc:" << QString(pAdapter->Description);
            QByteArray arr((char *)pAdapter->Address);
            m_macAddr[i].b1 = pAdapter->Address[0];
            m_macAddr[i].b1 = pAdapter->Address[1];
            m_macAddr[i].b1 = pAdapter->Address[2];
            m_macAddr[i].b1 = pAdapter->Address[3];
            m_macAddr[i].b1 = pAdapter->Address[4];
            m_macAddr[i].b1 = pAdapter->Address[5];
            ++i;
            qDebug() << "AdapterAddr:" << arr.toHex();
            qDebug() << "AdapterType" << pAdapter->Type;
            qDebug() << "IPAddress:" << QString(pAdapter->IpAddressList.IpAddress.String);
            qDebug() << "IPMask:"<< QString(pAdapter->IpAddressList.IpMask.String);
            //            }
            pAdapter = pAdapter->Next;
        }
    }
    else
    {
        // qDebug() << "CalltoGetAdaptersInfofailed.n";
    }
    //    qDebug() << "3";
    return i;
}
//***********************************************************************/
///* Fuction:        GetNetInfoOfWinNT()
///* Return:
///* PrameterList:
///* Description:
///***********************************************************************
BOOL CNetInfo::GetNetInfoOfWinNT()
{
    LONG lRtn;
    HKEY hMainKey;
    TCHAR szParameters[256];
    //获得域名，主机名和是否使用IP路由
    _tcscpy(szParameters,_T("SYSTEM\\ControlSet001\\Services\\Tcpip\\Parameters"));
    lRtn = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,szParameters,0,KEY_READ,&hMainKey);
    if(lRtn == ERROR_SUCCESS)
    {
        DWORD dwType,dwDataSize = 256;
        ::RegQueryValueEx(hMainKey,_T("Domain"),NULL,&dwType,(LPBYTE)m_szDomain,&dwDataSize);
        dwDataSize = 256;
        ::RegQueryValueEx(hMainKey,_T("Hostname"),NULL,&dwType,(LPBYTE)m_szHostName,&dwDataSize);
        dwDataSize = 256;
        //        BYTE szBuf[16];
        ::RegQueryValueEx(hMainKey,_T("IPEnableRouter"),NULL,&dwType,(LPBYTE)&m_IPEnableRouter,&dwDataSize);
    }
    ::RegCloseKey(hMainKey);

    //获得IP地址和DNS解析等其他设置
    HKEY hNetCard = NULL;
    m_totalNetCards = GetMacAddress();

    lRtn = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards"),0,KEY_READ,&hNetCard);
    if(lRtn != ERROR_SUCCESS)//此处失败就返回
    {
        if(hNetCard != NULL)
        {
            ::RegCloseKey(hNetCard);
        }
        return FALSE;
    }

    DWORD dwSubKeyNum = 0,dwSubKeyLen = 256;
    //得到子键的个数，通常与网卡个数相等
    lRtn = ::RegQueryInfoKey( hNetCard,NULL,NULL,NULL,&dwSubKeyNum,&dwSubKeyLen,
                             NULL,NULL,NULL,NULL,NULL,NULL);
    if(lRtn == ERROR_SUCCESS)
    {
        m_totalNetCards = dwSubKeyNum;//网卡个数以此为主
        LPTSTR lpszKeyName = new TCHAR[dwSubKeyLen + 1];
        DWORD dwSize;
        for(int i = 0; i < (int)dwSubKeyNum; i++)
        {
            TCHAR szServiceName[256];
            HKEY hNewKey;
            DWORD dwType = REG_SZ,dwDataSize = 256;
            dwSize = dwSubKeyLen + 1;
            ::RegEnumKeyEx(hNetCard,i,lpszKeyName,&dwSize,NULL,NULL,NULL,NULL);
            lRtn = ::RegOpenKeyEx(hNetCard,lpszKeyName,0,KEY_READ,&hNewKey);
            if(lRtn == ERROR_SUCCESS)
            {
                lRtn = ::RegQueryValueEx(hNewKey,_T("Description"),NULL,&dwType,(LPBYTE)m_cards[i].szDescription,&dwDataSize);
                dwDataSize = 256;
                lRtn = ::RegQueryValueEx(hNewKey,_T("ServiceName"),NULL,&dwType,(LPBYTE)szServiceName,&dwDataSize);
                if(lRtn == ERROR_SUCCESS)
                {
                    TCHAR szNewKey[256];
                    wsprintf(szNewKey,_T("%s\\Interfaces\\%s"),szParameters,szServiceName);
                    HKEY hTcpKey;
                    lRtn = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,szNewKey,0,KEY_READ,&hTcpKey);
                    if(lRtn == ERROR_SUCCESS)
                    {
                        dwDataSize = 256;
                        ::RegQueryValueEx(hTcpKey,_T("DefaultGateway"),NULL,&dwType,(LPBYTE)m_cards[i].szGateWay,&dwDataSize);
                        ProcessMultiString(m_cards[i].szGateWay,dwDataSize);
                        dwDataSize = 256;
                        ::RegQueryValueEx(hTcpKey,_T("IPAddress"),NULL,&dwType,(LPBYTE)m_cards[i].szIpAddress,&dwDataSize);
                        ProcessMultiString(m_cards[i].szIpAddress,dwDataSize);
                        dwDataSize = 256;
                        ::RegQueryValueEx(hTcpKey,_T("SubnetMask"),NULL,&dwType,(LPBYTE)m_cards[i].szIpMask,&dwDataSize);
                        ProcessMultiString(m_cards[i].szIpMask,dwDataSize);
                        dwDataSize = 256;
                        ::RegQueryValueEx(hTcpKey,_T("NameServer"),NULL,&dwType,(LPBYTE)m_cards[i].szDNSNameServer,&dwDataSize);
                    }
                    ::RegCloseKey(hTcpKey);
                }
            }
            ::RegCloseKey(hNewKey);
            //            m_cards[i].szMacAddr[0] = m_macAddr[i].b1;
            //            m_cards[i].szMacAddr[1] = m_macAddr[i].b2;
            //            m_cards[i].szMacAddr[2] = m_macAddr[i].b3;
            //            m_cards[i].szMacAddr[3] = m_macAddr[i].b4;
            //            m_cards[i].szMacAddr[4] = m_macAddr[i].b5;
            //            m_cards[i].szMacAddr[5] = m_macAddr[i].b6;
        }
        delete[] lpszKeyName;
    }
    ::RegCloseKey(hNetCard);
    return lRtn == ERROR_SUCCESS ? TRUE : FALSE;
}

//***********************************************************************/
///* Fuction:        GetAddressByIndex()
///* Return:
///* PrameterList:
///* Description:
///***********************************************************************
UCHAR CNetInfo::GetAddressByIndex(int lana_num, ASTAT &Adapter)
{
    NCB ncb;
    UCHAR uRetCode;
    memset(&ncb, 0, sizeof(ncb) );
    ncb.ncb_command = NCBRESET;
    ncb.ncb_lana_num = lana_num;
    //指定网卡号,首先对选定的网卡发送一个NCBRESET命令,以便进行初始化
    uRetCode = Netbios(&ncb );
    memset(&ncb, 0, sizeof(ncb) );
    ncb.ncb_command = NCBASTAT;
    ncb.ncb_lana_num = lana_num;   //指定网卡号
    strcpy((char *)ncb.ncb_callname,"*      " );
    ncb.ncb_buffer = (unsigned char *)&Adapter;
    //指定返回的信息存放的变量
    ncb.ncb_length = sizeof(Adapter);
    //接着,可以发送NCBASTAT命令以获取网卡的信息
    uRetCode = Netbios(&ncb );
    return uRetCode;
}
//***********************************************************************/
///* Fuction:        ProcessMultiString()
///* Return:
///* PrameterList:
///* Description:
///***********************************************************************
void CNetInfo::ProcessMultiString(LPTSTR lpszString, DWORD dwSize)
{
    for(int i = 0; i < int(dwSize - 2); i++)
    {
        if(lpszString[i] == _T('\0'))
        {
            lpszString[i] = _T(',');
        }
    }
}
