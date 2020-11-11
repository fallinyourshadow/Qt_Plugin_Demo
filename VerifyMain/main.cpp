#include "MainWindow.h"

#include <QApplication>
#include <PluginLoader.h>

#include <QSettings>
int main(int argc, char *argv[])
{
    QSettings machineCode("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Cryptography",QSettings::NativeFormat);
    qDebug() << machineCode.value("MachineGuid").toString();
    QApplication a(argc, argv);
    MainWindow w;
    PluginLoader l;
    l.setProperty("path","C:/Users/Administrator/Desktop/PluginDemo/GenSignature/Mydll/GenSignature.dll");
    //w.show();
    l.start();
   // w.show();
    //w.loadPlugin(QString::fromLocal8Bit("C:/Users/Administrator/Desktop/PluginDemo/Plugin/Mydll/Plugin.dll"));
    //w.loadPlugin(QString::fromLocal8Bit("C:/Users/Administrator/Desktop/PluginDemo/GenSignature/Mydll/GenSignature.dll"));
    return a.exec();
}
