#ifndef GENSIGNATURE_H
#define GENSIGNATURE_H
#include <QObject>
#include "GenSignature_global.h"
#include "CNetInfo.h"


class GENSIGNATURE_EXPORT GenSignature: public QObject, public GenSignatureInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID GEN_SIGNATURE_INTERFACE_IID FILE "GenSignature.json")//plugin's meta data. note: the JSON file is necessary，if you used this macro.
    Q_INTERFACES(GenSignatureInterface)
public:
    GenSignature();
    virtual void genSignature(QByteArray &outCode) Q_DECL_OVERRIDE;
private:
    //获取网络信息
    CNetInfo m_netInfo;
};

#endif // GENSIGNATURE_H
