#include "ESPNOW.h"
namespace ESPNOW
{
    bool LOG = true;
    int status = 0;
    void (*InputCallback)(RecvData_t) = NULL;

    uint8_t DeviceAddress[5][6];
    int DeviceNum = 0;

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
        RecvData_t msg;
        memcpy(&msg, incomingData, len);
        if (LOG)
        {
            char macStr[18];
            Serial.print("Packet from: ");
            snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                     mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
            Serial.println(macStr);
        }
        if (InputCallback != NULL)
        {
            if (msg.id == getIDbyMAC(mac_addr))
            {
                (*InputCallback)(msg);
            }
        }
    }
    void init()
    {
        // 初始化 ESP-NOW
        WiFi.mode(WIFI_STA);
        if (esp_now_init() != 0)
        {
            Serial.println("Error initializing ESP-NOW");
            return;
        }
        esp_now_register_send_cb(OnDataSent);
        esp_now_register_recv_cb(OnDataRecv);
    }
    void setInputCallback(void (*func)(RecvData_t))
    {
        InputCallback = func;
    }
    bool connectTo(uint8_t mac[6], int id, int type, int data)
    {
        SendData_t myData({id, type, data});

        if (id != getIDbyMAC(mac))
        {
            esp_now_peer_info_t peerInfo;
            memcpy(peerInfo.peer_addr, mac, 6);
            peerInfo.channel = 0;
            peerInfo.encrypt = false;
            peerInfo.ifidx = WIFI_IF_STA;

            if (esp_now_add_peer(&peerInfo) != ESP_OK)
            {
                Serial.println("Failed to add peer");
                return false;
            }
        }

        esp_err_t result = esp_now_send(mac, (uint8_t *)&myData, sizeof(myData));
        if (result == ESP_OK)
        {
            Serial.println("Sent with success");
            return true;
        }
        else
        {
            Serial.println("Error sending the data");
            return false;
        }
    }
    int addDevice(uint8_t mac[6])
    {
        if (getIDbyMAC(mac) != -1)
        {
            Serial.println("Aready Initalized!");
            return -1;
        }
        if (LOG)
        {
            char macStr[18];
            Serial.print("Trying to initalize: ");
            snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                     mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            Serial.println(macStr);
        }
        if (connectTo(mac, DeviceNum, 0, 0))
        {
            memcpy(DeviceAddress[DeviceNum], mac, 6);
            return DeviceNum++;
        }
        else
        {
            return -1;
        }
    }
    bool sendData(int id, int data)
    {
        uint8_t *mac = getMACbyID(id);
        if (LOG)
        {
            char macStr[18];
            Serial.print("Sending data to: ");
            snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                     mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            Serial.println(macStr);
        }
        if (connectTo(mac, id, 1, data))
            return true;
        else
            return false;
    }
    int getIDbyMAC(uint8_t mac[6])
    {
        for (int i = 0; i < DeviceNum; i++)
        {
            bool flag = true;
            for (int j = 0; j < 6; j++)
            {
                if (mac[j] != DeviceAddress[i][j])
                {
                    flag = false;
                    break;
                }
            }
            if (flag == true)
                return i;
        }
        return -1;
    }
    int getIDbyMAC(const uint8_t mac[6])
    {
        for (int i = 0; i < DeviceNum; i++)
        {
            bool flag = true;
            for (int j = 0; j < 6; j++)
            {
                if (mac[j] != DeviceAddress[i][j])
                {
                    flag = false;
                    break;
                }
            }
            if (flag == true)
                return i;
        }
        return -1;
    }

    uint8_t *getMACbyID(int id)
    {
        return DeviceAddress[id];
    }
    int String2Hex(std::string c)
    {
        int res = 0;
        Serial.println(c.c_str());

        if (c[0] >= 'A')
            res += (c[0] - 'A' + 10) * 16;
        else
            res += (c[0] - '0') * 16;

        if (c[1] >= 'A')
            res += (c[1] - 'A' + 10);
        else
            res += (c[1] - '0');
        return res;
    }
}