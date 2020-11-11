#ifndef CNETINFO_H
#define CNETINFO_H
#include <QObject>
#include <Windows.h>
#include <tchar.h>
#include "Nb30.h"

#pragma comment(lib,"Netapi32.lib")
#pragma comment(lib,"Advapi32.lib")
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"Iphlpapi.lib")

class CNetInfo : public QObject
{
    Q_OBJECT
    //操作系统类型
public:
    enum Win32Type
    {
        Unknow,                                    //未知
        Win32s,                                    //Win32
        Windows9X,                                //9X
        WinNT3,                                    //NT3
        WinNT4orHigher                            //NT4
    };

    typedef struct tagASTAT
    {
        ADAPTER_STATUS adapt;                    //适配器
        NAME_BUFFER    NameBuff [30];            //描述
    }ASTAT,*LPASTAT;

    //存储网卡的MAC地址的结构
    typedef struct tagMAC_ADDRESS
    {
        BYTE b1,b2,b3,b4,b5,b6;
    }MAC_ADDRESS,*LPMAC_ADDRESS;

    //网卡信息的数据结构，包括记录网卡的厂商及型号，与之绑定的IP地址，网关,DNS序列，子网掩码和物理地址
    typedef struct tagNET_CARD
    {
        TCHAR szDescription[256];                //描述
        //  BYTE  szMacAddr[6];                        //网卡地址
        TCHAR szGateWay[128];                    //网关地址
        TCHAR szIpAddress[128];                    //ip地址
        TCHAR szIpMask[128];                    //子网
        TCHAR szDNSNameServer[128];                //DNS
    }NET_CARD,*LPNET_CARD;

#define MAX_CARD  10                        //最大网卡数
    void        ProcessMultiString(LPTSTR lpszString,DWORD dwSize);
    UCHAR       GetAddressByIndex(int lana_num,ASTAT & Adapter);
    int         GetMacAddress();
    BOOL        GetNetInfo();
    Win32Type   GetSystemType();
    explicit    CNetInfo(QObject *parent = nullptr);
    virtual     ~CNetInfo();

    int totalNetCards();
    NET_CARD Cards(qint32 num);
    Win32Type systemType();
    MAC_ADDRESS macAddr(qint32 num);
    TCHAR *szHostName();
private:
    BOOL        GetNetInfoOfWinNT();
    BOOL        GetNetInfoOfWin9X();
    int         m_totalNetCards;            //系统的网卡数
    TCHAR       m_szDomain[16];                //域名
    TCHAR       m_szHostName[16];            //主机名
    int         m_IPEnableRouter;            //是否允许IP路由0-不允许,1-允许,2-未知
    int         m_enableDNS;                //是否允许DNS解析0-不允许,1-允许,2-未知
    NET_CARD    m_cards[MAX_CARD];            //默认的最大网卡数是10
    Win32Type   m_systemType;                //操作系统类型
    MAC_ADDRESS m_macAddr[MAX_CARD];        //允许10个网卡
signals:
};

#endif // CNETINFO_H
