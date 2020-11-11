/**
  ******************************************************************************
  * Copyright (C), 2020 -2021, Xxxxxx Tech. Co., Ltd.
  * File Name          :Plugin.h
  * Author             :ZhangRui
  * Version            :0.1
  * date               :2020/9/18
  * Description        :Create a Qt dynamic plugin
  * Function List      :
  * History            :
        <author>    <version>    <time>    <desc>
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 XXXX.
  * All rights reserved.</center></h2>
  *
  ******************************************************************************
  */
#ifndef PLUGINDEMO_H
#define PLUGINDEMO_H

#include "Plugin_global.h"
#include <QWidget>
#include <QObject>
#include <QPushButton>
//step 1: define a Pure Virtual Interface Class
class  PluginInterface
{
public:
    virtual ~PluginInterface() {}
    virtual void showWidget(QWidget * parent) = 0;
};

#define PLUGIN_INTERFACE_IID "io.qt.dynamicplugin"//plugin's iid
Q_DECLARE_INTERFACE(PluginInterface,PLUGIN_INTERFACE_IID)//register plugin iid to qt meta object

//step 2: define main body of plugin(inherited from interface and qt meta class).
class Q_DECL_EXPORT Plugin : public QWidget,public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PLUGIN_INTERFACE_IID FILE "plugin.json")//plugin's meta data. note: the JSON file is necessary，if you used this macro.
    Q_INTERFACES(PluginInterface)
public:
    QWidget * myWidget = nullptr;
    Plugin(){myWidget = new QWidget(this);}//note: if you used macro Q_PLUGIN_METADATA,that you must need a default constructor, otherwise will cause a compile error.
    explicit Plugin(QWidget *parent);
    QPushButton * button = nullptr;
    void showWidget(QWidget * parent) Q_DECL_OVERRIDE;//realize function showWidget
};
//step3 3: how to use, see VerifyMain
#endif // PLUGINDEMO_H
