#include "Plugin.h"
#include <QDebug>
#include <QMessageBox>

Plugin::Plugin(QWidget *parent): QWidget(parent)
{
    myWidget = new QWidget(parent);
}

void Plugin::showWidget(QWidget * parent)
{
    qDebug() << "plugin load success";
    myWidget = new QWidget(parent);
    button = new QPushButton(parent);
    button->setText("来自插件");
    button->show();
    QMessageBox::information(parent,"信息","插件安装成功");
    myWidget->setParent(parent, Qt::SubWindow);
    myWidget->show();
}
