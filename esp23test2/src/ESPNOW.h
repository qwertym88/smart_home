#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

namespace ESPNOW
{
    typedef struct
    {
        int id;         // 传入设备的ID
        int type;       // 1 普通消息 2 其他消息
        char data[256]; // 大于最长可能输入的字符串长度，事实上espnow最多只能传250的数据
    } RecvData_t;       //接收的数据类型

    typedef struct
    {
        int id;
        int type; // 0 初始化 1 传值
        int data; // 传的数据
    } SendData_t; //发送的数据类型

    extern int Status; // 状态
    extern bool LOG;   // 是否输出日志
    extern void (*InputCallback)(RecvData_t);

    extern uint8_t DeviceAddress[5][6]; // 最多5个
    extern int DeviceNum;               // 最多5个

    void init();
    void setInputCallback(void (*func)(RecvData_t));

    bool connectTo(uint8_t[6], int, int, int);
    int addDevice(uint8_t[6]); // 返回设备ID

    bool sendData(int, int); // 不成功返回false

    int getIDbyMAC(uint8_t[6]);
    int getIDbyMAC(const uint8_t[6]);
    uint8_t *getMACbyID(int);

    int String2Hex(std::string);
}