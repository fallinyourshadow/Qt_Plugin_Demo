#include "GenSignature.h"
#include <QDebug>
#include <iostream>
#define DEBUG(content) \
    qDebug() << __LINE__ << __FILE__ << content

GenSignature::GenSignature()
{

}

void GenSignature::genSignature(QByteArray &outCode)
{
    Q_UNUSED(outCode);
    CNetInfo::Win32Type type = m_netInfo.GetSystemType();
    DEBUG("系统类型");
    switch (type)
    {
    case CNetInfo::Unknow:
        DEBUG("Unknow");
        break;
    case CNetInfo::Win32s:
        DEBUG("Win32s");
        break;
    case CNetInfo::Windows9X:
        DEBUG("Windows9X");
        break;
    case CNetInfo::WinNT3:
        DEBUG("WinNT3");
        break;
    case CNetInfo::WinNT4orHigher:
        DEBUG("WinNT4orHigher");
        break;//NT4
    default:
        break;
    }
    if(!m_netInfo.GetNetInfo())
    {
        DEBUG("获取网络信息失败");
        return;
    }
    DEBUG("获取网络信息成功");

    DEBUG("主机名称:" << QString::fromWCharArray(m_netInfo.szHostName()));
    int cardCount = m_netInfo.totalNetCards();
    DEBUG("网卡数量:" << cardCount);
    for(int i = 0; i < cardCount ; ++i)
    {
//        DEBUG(i << ".mac:"
//                << m_netInfo.macAddr(i).b1
//                << m_netInfo.macAddr(i).b2
//                << m_netInfo.macAddr(i).b3
//                << m_netInfo.macAddr(i).b4
//                << m_netInfo.macAddr(i).b5
//                << m_netInfo.macAddr(i).b6);
        DEBUG(i << ".描述:" <<  QString::fromWCharArray(m_netInfo.Cards(i).szDescription)) ;
    }
    // m_netInfo.GetMacAddress();
}
