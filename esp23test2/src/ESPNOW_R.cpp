#include "ESPNOW_R.h"

namespace ESPNOW_R
{
    int ID = 0;                                    // ID
    int Status = 0;                                // 状态 0 未初始化 1 已初始化
    bool LOG = true;                               // 是否输出日志
    uint8_t HostMacAdress[6] = {0, 0, 0, 0, 0, 0}; // 主机地址
    void (*InputCallback)(int) = NULL;
    void (*InitCallback)() = NULL;

    void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
    {
        if (LOG)
        {
            char macStr[18];
            Serial.print("Packet to: ");
            snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                     mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
            Serial.println(macStr);
        }
        Serial.print("Send status: ");
        Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
        Serial.println();
    }
    // 数据接收回调函数
    void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
    {
        RecvMsg_t msg;
        memcpy(&msg, incomingData, len);
        if (msg.type == 0)
        {
            bool ReInitalized = true;
            for (int i = 0; i < 6; i++)
            {
                if (mac_addr[i] != HostMacAdress[i])
                {
                    ReInitalized = false;
                    break;
                }
            }
            if (ReInitalized)
            {
                ID = msg.id;
                return;
            }

            if (LOG)
            {
                char macStr[18];
                Serial.print("Initalized by: ");
                snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                         mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
                Serial.println(macStr);
            }

            esp_now_peer_info_t peerInfo;
            memcpy(peerInfo.peer_addr, mac_addr, 6);
            peerInfo.channel = 0;
            peerInfo.encrypt = false;
            peerInfo.ifidx = WIFI_IF_STA;
            if (esp_now_add_peer(&peerInfo) != ESP_OK)
            {
                Serial.println("Failed to Initalized!");
            }
            else
            {
                memcpy(HostMacAdress, mac_addr, 6);
                ID = msg.id;
                Status = true;
                if (LOG)
                    Serial.println("Initalized Successfully!");
            }
            delay(3000);
            (*InitCallback)();
        }
        else if (msg.type == 1)
        {
            if (InputCallback != NULL)
            {
                (*InputCallback)(msg.data);
            }
        }
    }
    void init()
    {
        WiFi.mode(WIFI_STA);
        if (esp_now_init() != ESP_OK)
        {
            Serial.println("Error initializing ESP-NOW");
        }

        esp_now_register_recv_cb(OnDataRecv);
        esp_now_register_send_cb(OnDataSent);
    }
    void setInputCallback(void (*func)(int))
    {
        InputCallback = func;
    }
    void setInitalizedCallback(void (*func)(void))
    {
        InitCallback = func;
    }

    bool sendData(const char *str, int len, int type)
    {
        SendMsg_t msg;
        msg.id = ID;
        msg.type = type;
        memcpy(msg.data, str, len);

        if (len >= __MAX_SIZE_TO_SEND)
        {
            Serial.println("Message to send is too large!");
            Serial.println("Please send string less than 220 bytes");
            return false;
        }

        if (LOG)
        {
            Serial.print("Message: ");
            Serial.println(str);
        }

        esp_err_t result = esp_now_send(HostMacAdress, (uint8_t *)&msg, sizeof(msg));
        if (result == ESP_OK)
        {
            if (LOG)
                Serial.println("Sent with success");
            return true;
        }
        else
        {
            Serial.println("Error sending the data");
            return false;
        }
    }
    bool sendData(std::string str, int type)
    {
        return sendData(str.c_str(), str.size() * sizeof(char), type);
    }
    bool sendData(String str, int type)
    {
        return sendData(str.c_str(), str.length() * sizeof(char), type);
    }
}