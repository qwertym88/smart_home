#include "BLE.h"
namespace BLE
{
    // 生成 UUID: https://www.uuidgenerator.net/
    const char *SERVICE_UUID = "9a712029-4bd1-4b46-bbcc-07f61a4b3268"; // service UUID
    const char *UUID_RX = "7d63c7ee-a016-451c-bbdf-8a41463e2f8d";      // 读特征的 UUID
    const char *UUID_TX = "882eae86-c1e0-497b-8941-e081faaa68c5";      // 写特征的 UUID
    const char *UUID_NOTIFY = "a9656b46-5075-47f3-afc9-122b05890910";  // 通知特征的 UUID

    BLEServer *pServer = NULL;              // BLEServer指针 pServer
    BLECharacteristic *pReadChara = NULL;   // BLECharacteristic指针 用来实现live load部分
    BLECharacteristic *pNotifyChara = NULL; // BLECharacteristic指针 用来实现通知部分

    bool CONNECTED = false; //本次连接状态
    bool LOG = true;        // 是否打印日志

    void (*InputCallback)(std::string) = NULL;
    void (*ConnectedCallback)() = NULL;

    class MyServerCallbacks : public BLEServerCallbacks
    {
        void onConnect(BLEServer *pServer)
        {
            CONNECTED = true;
            if (LOG)
                Serial.println("Connected!");
            if (ConnectedCallback != NULL)
                (*ConnectedCallback)();
        };

        void onDisconnect(BLEServer *pServer)
        {
            CONNECTED = false;
            if (LOG)
                Serial.println("Disconnected! Start Advertising...");
            BLEDevice::startAdvertising();
        }
    };
    class MyCallbacks : public BLECharacteristicCallbacks
    {
        void onWrite(BLECharacteristic *pCharacteristic)
        {
            std::string inputValue = pCharacteristic->getValue(); //接收信息

            // P.S 这么搞纯粹是因为Serial.println(inputValue);会报错
            if (inputValue.length() > 0)
            { //向串口输出收到的值
                if (BLE::LOG)
                {
                    Serial.print("Input: ");
                    for (int i = 0; i < inputValue.length(); i++)
                        Serial.print(inputValue[i]);
                    Serial.println();
                }
                ///////////////////////////////////////////
                /*
                  写处理接收到inputValue后要干什么
                */
                if (BLE::InputCallback != NULL)
                {
                    (*BLE::InputCallback)(inputValue);
                }
            }
        }
    };

    void init()
    {
        // 创建一个 BLE 设备
        BLEDevice::init("MY_ESP32");

        // 创建一个 BLE 服务
        pServer = BLEDevice::createServer();
        pServer->setCallbacks(new MyServerCallbacks()); //设置回调
        BLEService *pService = pServer->createService(SERVICE_UUID);

        // 创建一个读和通知特征，供其他设备读取数据或接收通知 pReadChara->setValue() 设置值，供读取  notify()提醒订阅通知了的设备读取
        // indicate 特征 和 notify 一模一样，只不过前者有确认接收到提醒的握手行为
        pReadChara = pService->createCharacteristic(UUID_TX, BLECharacteristic::PROPERTY_READ);
        pReadChara->addDescriptor(new BLE2902()); //可以是其他的值，2903什么的，详见https://blog.csdn.net/chengdong1314/article/details/70167954

        pNotifyChara = pService->createCharacteristic(UUID_NOTIFY, BLECharacteristic::PROPERTY_NOTIFY);
        pNotifyChara->addDescriptor(new BLE2902());

        // 创建写特征值，用于写入数据
        BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(UUID_RX, BLECharacteristic::PROPERTY_WRITE);
        pRxCharacteristic->setCallbacks(new MyCallbacks()); //设置回调

        pService->start();                  // 开始服务
        pServer->getAdvertising()->start(); // 开始广播
        Serial.println(" Waiting for connection... ");
    }

    void setInputCallback(void (*func)(std::string))
    {
        InputCallback = func;
    }
    void setConnectedCallback(void (*func)())
    {
        ConnectedCallback = func;
    }

    void update(std::string data)
    {
        notification("123");
        pReadChara->setValue(data);
    }
    void notification(std::string msg)
    {
        pNotifyChara->setValue(msg);
        pNotifyChara->notify();
    }
}
