#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

#define __MAX_SIZE_TO_SEND 220 // 最大数据量，espnow最多只能传250的数据

namespace ESPNOW_R
{
    typedef struct
    {
        int id;   // 分配的ID
        int type; // 0 初始化 1 发送数据
        int data; // 数据 int
    } RecvMsg_t;
    typedef struct
    {
        int id;   // 本机的ID
        int type; // 1 普通消息 2 其他消息
        char data[__MAX_SIZE_TO_SEND];
    } SendMsg_t;

    extern int ID;                   // ID
    extern int Status;               // 状态 0 未初始化 1 已初始化
    extern bool LOG;                 // 是否输出日志
    extern uint8_t HostMacAdress[6]; // 主机地址
    extern void (*InputCallback)(int);
    extern void (*InitCallback)();

    void init();
    void setInputCallback(void (*func)(int));
    void setInitalizedCallback(void (*func)(void));
    bool sendData(std::string, int);
    bool sendData(String, int);
    bool sendData(const char *, int, int);
}