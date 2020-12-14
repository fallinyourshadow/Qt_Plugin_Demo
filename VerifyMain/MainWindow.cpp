#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QPluginLoader>
#include "PluginInterface.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadPlugin(QString pluginPath)
{
    QFile pluginFile(pluginPath);
    if(!pluginFile.exists())
    {
        QMessageBox::warning(this,"警告","找不到指定的插件");
        return;
    }
    else
    {
        PluginInterface *interface = nullptr;
        QPluginLoader loader(pluginPath);
        QObject *pluginInstance = loader.instance();
        if(pluginInstance)
        {
            //qDebug() << __FUNCDNAME__ << pluginInstance;
            interface = qobject_cast<PluginInterface*>(pluginInstance);
        }
       // loader.dumpObjectInfo();
        //qDebug() << __FUNCDNAME__ << loader.unload();
        return;
    }
}


