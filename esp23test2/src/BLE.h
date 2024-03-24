#pragma once
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <string>
namespace BLE
{
    extern bool CONNECTED;
    extern bool LOG;
    extern void (*InputCallback)(std::string);
    extern void (*ConnectedCallback)();

    void init();                                      //初始化，先调用setInputCallback后再调用这个
    void setInputCallback(void (*func)(std::string)); // 设置接收信息后要干什么
    void setConnectedCallback(void (*func)());        // 设置连接后干什么，主要用于及时更新数据
    void update(std::string);                         // 更新数据
    void notification(std::string);                   // 发送通知
}