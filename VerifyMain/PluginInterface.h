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

#include <QWidget>


//step 1: define a Pure Virtual Interface Class
class  PluginInterface
{
public:
    virtual ~PluginInterface() {}
    virtual void showWidget(QWidget * parent) = 0;
};

#define PLUGIN_INTERFACE_IID "io.qt.dynamicplugin"//plugin's iid
Q_DECLARE_INTERFACE(PluginInterface, PLUGIN_INTERFACE_IID)//register plugin iid to qt meta object

#endif // PLUGINDEMO_H
