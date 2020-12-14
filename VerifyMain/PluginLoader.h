#ifndef QPLUGINLOADERTHREAD_H
#define QPLUGINLOADERTHREAD_H
#include "MainWindow.h"
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QPluginLoader>
#include <QThread>
#include "../GenSignature/GenSignature_global.h"

class PluginLoader: public QThread
{
    Q_OBJECT
signals:
    void loadDone(QObject * plugin);

public:
    PluginLoader(){
        w = new MainWindow(nullptr);
        w->show();
        connect(this,&PluginLoader::loadDone,this,&PluginLoader::on_loadDone);
    }
    ~PluginLoader()
    {
       delete w;
    }
    virtual void run(){
        QString path = property("path").value<QString>();
        Q_EMIT loadDone(loadPlugin(path));
    }
    //新的插件重写这个函数
    void on_loadDone(QObject * plugin){
        if(nullptr != plugin)
        {
            GenSignatureInterface *pIf = qobject_cast<GenSignatureInterface *>(plugin);
            QByteArray byteArray;
            pIf->genSignature(byteArray);
        }
    }
    QObject* loadPlugin(QString pluginPath)
    {
        QObject *interface = nullptr;
        QFile pluginFile(pluginPath);
        if(!pluginFile.exists())
        {
            //qDebug() << __FUNCDNAME__ << pluginPath;
            QMessageBox::warning(nullptr,QString::fromLocal8Bit( "warning"),QString::fromLocal8Bit("dll not fund"));
        }
        else
        {
            QPluginLoader loader(pluginPath);
            QObject *pluginInstance = loader.instance();
            if(pluginInstance)
            {
                //qDebug() << __FUNCDNAME__ << pluginInstance;
                interface = pluginInstance;
            }
            // loader.dumpObjectInfo();
            //qDebug() << __FUNCDNAME__ << loader.unload();
        }
        return interface;
    }
private:
    MainWindow * w;
};


#endif // QPLUGINLOADERTHREAD_H
