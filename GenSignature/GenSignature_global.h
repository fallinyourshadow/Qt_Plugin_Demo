#ifndef GENSIGNATURE_GLOBAL_H
#define GENSIGNATURE_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QObject>
#if defined(GENSIGNATURE_LIBRARY)
#  define GENSIGNATURE_EXPORT Q_DECL_EXPORT
#else
#  define GENSIGNATURE_EXPORT Q_DECL_IMPORT
#endif

class GenSignatureInterface
{
public:
    virtual ~GenSignatureInterface() {}
    //生成特征码
    virtual void genSignature(QByteArray &outCode) = 0;
};

#define GEN_SIGNATURE_INTERFACE_IID "io.qt.gensignature"

Q_DECLARE_INTERFACE(GenSignatureInterface,GEN_SIGNATURE_INTERFACE_IID)
#endif // GENSIGNATURE_GLOBAL_H
