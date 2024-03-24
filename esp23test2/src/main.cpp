// #include <Arduino.h>
// #include "ESPNOW_R.h"
// void sendData(){
//     char msg[80];
//     sprintf(msg, "{\"name\":\"Light\",\"data\":{}");
//     ESPNOW_R::sendData(msg, sizeof(msg), 1);
// }
// void InputHandler(int command)
// {
//     if (command == 114514){ // 系统固定,用于更新数据的代码
//         sendData();
//     }
//     if (command == 0){
//         // 关灯
//     }
//     if (command == 1){
//         // 开灯
//     }
// }
// void AfterInitalized(){
//     sendData();
// }
// void setup(){
//     Serial.begin(115200);

//     ESPNOW_R::LOG = true;
//     ESPNOW_R::setInitalizedCallback(AfterInitalized);
//     ESPNOW_R::setInputCallback(InputHandler);
//     ESPNOW_R::init();
// }
// void loop(){}

/////////////////////////////////////////////////////// 3代魔改版wifi，大获全胜
#include <Arduino.h>
#include "BLE.h"
#include "ESPNOW.h"

void ESPInputHandler(ESPNOW::RecvData_t msg) // 处理从ESP-now（分模块）处接收到的数据
{
    if (msg.type == 1) // 普通消息
    {
        if (BLE::CONNECTED)
        {
            std::string usual_msg1 = "{\"type\":\"usual\",\"devicesNum\":";
            usual_msg1 = usual_msg1 + (char)(ESPNOW::DeviceNum + '0');
            usual_msg1 = usual_msg1 + ",\"thisDeviceID\":" + (char)(msg.id + '0');
            usual_msg1 = usual_msg1 + ",\"deviceData\":";
            std::string data = msg.data;
            std::string usual_msg2 = "}";
            Serial.println(msg.data);
            std::string all = usual_msg1 + data + usual_msg2;
            Serial.println(all.c_str());
            BLE::update(usual_msg1 + data + usual_msg2);
        }
    }
    if (msg.type == 2) // 紧急消息
    {
    }
}

void BLEInputHandler(std::string input) // 处理从蓝牙（手机）处接收到的数据
{
    char Command = input[0];
    if (Command == 'C') // 连接模块
    {
        uint8_t targetMAC[6];
        for (int i = 0; i < 6; i++)
        {
            targetMAC[i] = ESPNOW::String2Hex(input.substr(2 + i * 3, 2));
        }
        for (int i = 0; i < 6; i++)
        {
            Serial.println(targetMAC[i]);
        }
        int id = ESPNOW::addDevice(targetMAC);
        if (id != -1)
        {
            Serial.print("Connected! id=");
            Serial.println(id);
        }
    }
    if (Command == 'R') // 发送数据
    {
        int id = atoi(input.substr(1).c_str());
        int data = atoi(input.substr(4).c_str());
        Serial.print("Send to : ");
        Serial.println(id);
        Serial.print("Send : ");
        Serial.println(data);
        int res = ESPNOW::sendData(id, data);
        if (res == -1)
        {
            Serial.println("Send Failed");
        }
    }
}

void BLEConnectedHandler() // 重新连接时，让所有子模块发送一次数据
{
    for (int i = 0; i < ESPNOW::DeviceNum; i++)
    {
        ESPNOW::sendData(i, 114514);
        delay(500);
    }
}

void setup()
{
    Serial.begin(115200);
    BLE::LOG = true;
    BLE::setConnectedCallback(BLEConnectedHandler);
    BLE::setInputCallback(BLEInputHandler);
    BLE::init();

    // 初始化 ESP-NOW
    ESPNOW::LOG = true;
    ESPNOW::setInputCallback(ESPInputHandler);
    ESPNOW::init();
}

void loop()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////// 3代魔改版DHT代码，大获全胜
// #include <Arduino.h>
// #include <WiFi.h>
// #include <esp_now.h>
// #include <DFRobot_DHT11.h>
// #include "ESPNOW_R.h"

// DFRobot_DHT11 DHT;
// int inf;
// bool button = 0;
// bool enabled = true;
// bool switc = false;

// unsigned long int lastT = 0;
// unsigned long int lastTt = 0;
// int humi = 50;

// void InputHandler(int data)
// {
//     Serial.print("Get code ");
//     Serial.println(data);
//     if (data == 114514) // 更新代码
//     {
//         char msg[80];
//         sprintf(msg, "{\"name\":\"DHTsensor\",\"data\":{\"temp\":%d,\"humi\":%d,\"status\":%d}}", DHT.temperature, DHT.humidity, button);
//         ESPNOW_R::sendData(msg, sizeof(msg), 1);
//     }
//     if (data == 404) // 打开开关代码
//     {
//         enabled = false;
//         switc = true;
//     }
//     if (data == 504) // 关闭开关代码
//     {
//         enabled = false;
//         switc = false;
//     }
//     if (data == 200) // 打开自动模式代码
//     {
//         enabled = true;
//     }
// }

// void AfterInitalized()
// {
//     char msg[80];
//     sprintf(msg, "{\"name\":\"DHTsensor\",\"data\":{\"temp\":%d,\"humi\":%d,\"status\":%d}}", DHT.temperature, DHT.humidity, button);
//     ESPNOW_R::sendData(msg, sizeof(msg), 1);
// }

// void setup()
// {
//     Serial.begin(115200);
//     pinMode(32, OUTPUT);
//     pinMode(33, INPUT);

//     ESPNOW_R::LOG = true;
//     ESPNOW_R::setInitalizedCallback(AfterInitalized);
//     ESPNOW_R::setInputCallback(InputHandler);
//     ESPNOW_R::init();
// }

// void loop()
// {
//     if (millis() - lastTt >= 5000)
//     {
//         DHT.read(33);
//         humi = DHT.humidity;
//         Serial.println(humi);
//         lastTt = millis();
//     }
//     if (enabled)
//     {
//         if (humi <= 65 && button == 0)
//         {
//             digitalWrite(32, HIGH);
//             button = 1;
//         }
//         if (humi >= 75 && button == 1)
//         {
//             digitalWrite(32, LOW);
//             button = 0;
//         }
//     }
//     else
//     {
//         if (switc)
//         {
//             digitalWrite(32, HIGH);
//             button = 1;
//         }
//         else
//         {
//             digitalWrite(32, LOW);
//             button = 0;
//         }
//     }
//     if (millis() - lastT >= 30000) // 什么都没发生，则半分钟更新一次
//     {
//         if (ESPNOW_R::Status == 1)
//         {
//             char msg[100];
//             sprintf(msg, "{\"name\":\"DHTsensor\",\"data\":{\"temp\":%d,\"humi\":%d,\"status\":%d}}", DHT.temperature, DHT.humidity, button);
//             ESPNOW_R::sendData(msg, sizeof(msg), 1);
//         }
//         lastT = millis();
//     }
// }

// ///////////////////////////////////////////////////////////////////// 2代魔改版垃圾桶代码，大获成功
// #include <Arduino.h>
// #include <ESP32Servo.h>
// #include <WiFi.h>
// #include <esp_now.h>
// #include <string.h>
// #include <sstream>
// #include "ESPNOW_R.h"

// Servo myservo_1; //定义一个Servo对象控制 有害垃圾舵机
// Servo myservo_2; //定义一个Servo对象控制 可回收垃圾舵机
// Servo myservo_3; //定义一个Servo对象控制 厨余垃圾舵机
// Servo myservo_4; //定义一个Servo对象控制 其他垃圾舵机

// int a; //接收串口获得的数据

// int pushButton1 = 16; //可回收
// int pushButton2 = 17; //厨余
// int pushButton3 = 18; //其他
// int pushButton4 = 19; //有害

// // const std::string waste[4][4] = {{"battery", "capsule", "alcohol", "injector"},
// //                                  {"newpaper", "spon", "plastic", "teethbrush"},
// //                                  {"strawberry", "bread", "mushroom", "onion"},
// //                                  {"sponge", "tissue", "fpot", "mud"}};
// // const std::string type[] = {"Harmful", "Recyclable", "Kitchen", "bbb"};

// // 精简一下名称
// const std::string waste[4][4] = {{"bat", "cap", "alc", "inj"},
//                                  {"nep", "spn", "pla", "te"},
//                                  {"stw", "bre", "mus", "oni"},
//                                  {"spg", "tis", "fpo", "mud"}};
// const std::string type[] = {"har", "rec", "ktc", "bbb"};

// int total[4][4];

// std::string data()
// {
//     std::stringstream sstrg;
//     sstrg.str("");
//     sstrg << "{\"name\":\"Bin\",\"data\":{\"load\":{";
//     for (int i = 0; i < 3; i++)
//     {
//         sstrg << '\"' << type[i] << "\":{";
//         for (int j = 0; j < 3; j++)
//         {
//             sstrg << '\"' << waste[i][j] << "\":" << total[i][j] << ',';
//         }
//         sstrg << '\"' << waste[i][3] << "\":" << total[i][3] << "},";
//     }
//     sstrg << '\"' << type[3] << "\":{";
//     for (int j = 0; j < 3; j++)
//     {
//         sstrg << '\"' << waste[3][j] << "\":" << total[3][j] << ',';
//     }
//     sstrg << '\"' << waste[3][3] << "\":" << total[3][3];
//     sstrg << "}}}}";
//     return sstrg.str();
// }

// void InputHandler(int c)
// {
//     if (c == 114514)
//     {
//         ESPNOW_R::sendData(data(), 1);
//     }
// }
// void AfterInitalized()
// {
//     ESPNOW_R::sendData(data(), 1);
// }

// void setup()
// {
//     pinMode(pushButton1, INPUT);
//     pinMode(pushButton2, INPUT);
//     pinMode(pushButton3, INPUT);
//     pinMode(pushButton4, INPUT);
//     Serial.begin(9600); //设置串口波特率为9600buad
//     for (int i = 0; i < 4; i++)
//         for (int j = 0; j < 4; j++)
//             total[i][j] = 0;

//     // 初始化 ESP-NOW
//     ESPNOW_R::setInputCallback(InputHandler);
//     ESPNOW_R::setInitalizedCallback(AfterInitalized);
//     ESPNOW_R::init();

//     //舵机初始化
//     myservo_1.attach(25); // 控制线连接数字25  有害垃圾舵机
//     myservo_1.write(0);
//     myservo_2.attach(26); // 控制线连接数字26   可回收物舵机
//     myservo_2.write(0);
//     myservo_3.attach(27); // 控制线连接数字27   厨余垃圾舵机
//     myservo_3.write(0);
//     myservo_4.attach(33); // 控制线连接数字33   其他垃圾舵机
//     myservo_4.write(0);

//     delay(3000); //每次上电需延时3秒钟，等待M6SE模块初始化，这个延时不能省略。
// }

// void Servoturn1()
// {
//     myservo_1.write(90); // 舵机角度写入
//     delay(5);            // 等待转动到指定角度
//     delay(3500);         //等待丢垃圾时间，设置为3秒
//     myservo_1.write(0);  // 舵机角度写入
//     delay(5);            // 等待转动到指定角度

//     if (ESPNOW_R::Status == 1)
//     {
//         ESPNOW_R::sendData(data(), 1);
//     }
// }

// void Servoturn2()
// {
//     myservo_2.write(90); // 舵机角度写入
//     delay(5);            // 等待转动到指定角度
//     delay(3500);         //等待丢垃圾时间，设置为3秒
//     myservo_2.write(0);  // 舵机角度写入
//     delay(5);            // 等待转动到指定角度

//     if (ESPNOW_R::Status == 1)
//     {
//         ESPNOW_R::sendData(data(), 1);
//     }
// }

// void Servoturn3()
// {
//     myservo_3.write(90); // 舵机角度写入
//     delay(5);            // 等待转动到指定角度
//     delay(3500);         //等待丢垃圾时间，设置为3秒
//     myservo_3.write(0);  // 舵机角度写入
//     delay(5);            // 等待转动到指定角度

//     if (ESPNOW_R::Status == 1)
//     {
//         ESPNOW_R::sendData(data(), 1);
//     }
// }

// void Servoturn4()
// {
//     myservo_4.write(90); // 舵机角度写入
//     delay(5);            // 等待转动到指定角度
//     delay(3500);         //等待丢垃圾时间，设置为3秒
//     myservo_4.write(0);  // 舵机角度写入
//     delay(5);            // 等待转动到指定角度

//     if (ESPNOW_R::Status == 1)
//     {
//         ESPNOW_R::sendData(data(), 1);
//     }
// }

// void loop() //循环执行
// {
//     int buttonState1 = digitalRead(pushButton1);
//     int buttonState2 = digitalRead(pushButton2);
//     int buttonState3 = digitalRead(pushButton3);
//     int buttonState4 = digitalRead(pushButton4);

//     a = Serial.read();
//     switch (a)
//     {
//     case 0x01:
//         total[0][0]++;
//         Servoturn1();
//         break;
//     case 0x02:
//         total[0][1]++;
//         Servoturn1();
//         break;
//     case 0x03:
//         total[0][2]++;
//         Servoturn1();
//         break;
//     case 0x04:
//         total[0][3]++;
//         Servoturn1();
//         break;
//     case 0x05:
//         total[1][0]++;
//         Servoturn2();
//         break;
//     case 0x06:
//         total[1][1]++;
//         Servoturn2();
//         break;
//     case 0x07:
//         total[1][2]++;
//         Servoturn2();
//         break;
//     case 0x08:
//         total[1][3]++;
//         Servoturn2();
//         break;
//     case 0x09:
//         total[2][0]++;
//         Servoturn3();
//         break;
//     case 0x0A:
//         total[2][1]++;
//         Servoturn3();
//         break;
//     case 0x0B:
//         total[2][2]++;
//         Servoturn3();
//         break;
//     case 0x0C:
//         total[2][3]++;
//         Servoturn3();
//         break;
//     case 0x0D:
//         total[3][0]++;
//         Servoturn4();
//         break;
//     case 0x0E:
//         total[3][1]++;
//         Servoturn4();
//         break;
//     case 0x0F:
//         total[3][2]++;
//         Servoturn4();
//         break;
//     case 0x10:
//         total[3][3]++;
//         Servoturn4();
//         break;
//     }

//     if (buttonState1 == 0)
//     {
//         for (int i = 0; i < 4; i++)
//         {
//             total[1][i] = 0;
//         }
//         if (ESPNOW_R::Status == 1)
//         {
//             ESPNOW_R::sendData(data(), 1);
//         }
//         delay(1000);
//     }
//     if (buttonState2 == 0)
//     {
//         for (int i = 0; i < 4; i++)
//         {
//             total[2][i] = 0;
//         }
//         if (ESPNOW_R::Status == 1)
//         {
//             ESPNOW_R::sendData(data(), 1);
//         }
//         delay(1000);
//     }
//     if (buttonState3 == 0)
//     {
//         for (int i = 0; i < 4; i++)
//         {
//             total[3][i] = 0;
//         }
//         if (ESPNOW_R::Status == 1)
//         {
//             ESPNOW_R::sendData(data(), 1);
//         }
//         delay(1000);
//     }
//     if (buttonState4 == 0)
//     {
//         for (int i = 0; i < 4; i++)
//         {
//             total[0][i] = 0;
//         }
//         if (ESPNOW_R::Status == 1)
//         {
//             ESPNOW_R::sendData(data(), 1);
//         }
//         delay(1000);
//     }
// }

// // /////////////////////////////////////////////////////////////////// 魔改版门禁
// #include <Arduino.h>
// #define FS5 740
// #define D5 587
// #define A4 440
// #define E5 659
// #define A5 880
// #include <Stepper.h>
// #include "ESPNOW_R.h"

// const int STEPS_PER_ROTOR_REV = 32;
// const int GEAR_REDUCTION = 64;
// const float STEPS_PER_OUT_REV = STEPS_PER_ROTOR_REV * GEAR_REDUCTION;
// const int r = 32;
// const int g = 33;
// int Steps;
// int phone = 1;
// Stepper steppermotor(STEPS_PER_ROTOR_REV, 26, 27, 14, 12);
// const int louder = 22;
// const int infr = 35;
// const int trigPin = 16; // Trig
// const int echoPin = 17; // Echo
// long cm;
// const int bpm = 60000 / 180;
// const int soundJudge = 2;

// int melody[] = {FS5, D5, A4, D5, E5, A5, 0, E5, FS5, E5, A4, D5, 0};
// int melody2[] = {A4, A4, A4, A4};
// int durations[] = {1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2};
// int k = 0;
// int times1 = 0;
// int ENTERED = 0;

// void playHome()
// {
//     int i;
//     for (i = 0; i <= 12; i++)
//     {
//         ledcWriteTone(0, melody[i]);
//         delay(bpm * durations[i]);
//     }
// }

// void playWarning()
// {
//     ledcWriteTone(0, A4);
//     delay(1500);
//     ledcWriteTone(0, 0);
// }

// long distance()
// {
//     long duration, cm, inches;
//     digitalWrite(trigPin, LOW);
//     delayMicroseconds(5);
//     digitalWrite(trigPin, HIGH);
//     delayMicroseconds(10);
//     digitalWrite(trigPin, LOW);
//     duration = pulseIn(echoPin, HIGH);
//     cm = (duration / 2) / 29.1;
//     inches = (duration / 2) / 74;
//     Serial.print(inches);
//     Serial.print("in, ");
//     Serial.print(cm);
//     Serial.print("cm");
//     Serial.println();
//     return cm;
//     delay(1000);
// }

// void sendMessage(int i, int a)
// {
//     if (ESPNOW_R::Status == 1)
//     {
//         char msg[100];
//         sprintf(msg, "{\"name\":\"Door\",\"data\":{\"times\":%d,\"status\":%d}}", times1, a);
//         ESPNOW_R::sendData(msg, sizeof(msg), 1);
//     }
//     delay(1000);
// }

// void InputHandler(int command)
// {
//     if (command == 114514) // 更新代码
//     {
//         char msg[100];
//         sprintf(msg, "{\"name\":\"Door\",\"data\":{\"times\":%d,\"status\":%d}}", times1, ENTERED);
//         ESPNOW_R::sendData(msg, sizeof(msg), 1);
//     }
//     if (command == 404) // 关闭门禁
//     {
//         phone = 0;
//     }
// }
// void AfterInitalized()
// {
//     char msg[100];
//     sprintf(msg, "{\"name\":\"Door\",\"data\":{\"times\":%d,\"status\":%d}}", times1, ENTERED);
//     ESPNOW_R::sendData(msg, sizeof(msg), 1);
// }

// void setup()
// {
//     Serial.begin(115200);
//     pinMode(infr, INPUT);
//     pinMode(r, OUTPUT);
//     pinMode(g, OUTPUT);
//     pinMode(trigPin, OUTPUT);
//     pinMode(echoPin, INPUT);
//     pinMode(soundJudge, INPUT);
//     ledcSetup(0, 20000, 10);
//     ledcAttachPin(louder, 0);

//     ESPNOW_R::setInputCallback(InputHandler);
//     ESPNOW_R::setInitalizedCallback(AfterInitalized);
//     ESPNOW_R::init();
// }

// void loop()
// {
//     bool judge = digitalRead(infr);
//     cm = distance();
//     Serial.println(digitalRead(soundJudge));
//     if (phone == 1)
//     {
//         if (judge && cm <= 50)
//         {
//             digitalWrite(r, HIGH);
//             playWarning();
//             judge = 0;
//             k++; //这个是返回到手机app的值之一，表示触发次数
//             if (digitalRead(soundJudge) == HIGH)
//             {
//                 times1 += k;
//                 playHome();
//                 sendMessage(times1, 1);
//                 ENTERED = 1;
//                 Steps = STEPS_PER_OUT_REV;
//                 steppermotor.setSpeed(500);
//                 steppermotor.step(Steps);
//                 k = 0;
//             }
//             if (k == 3)
//             {
//                 int aa;
//                 times1 = times1 + k;
//                 sendMessage(times1, 0);
//                 ENTERED = 0;
//                 for (aa = 0; aa < 2; aa++)
//                 {
//                     playWarning();
//                 }
//                 delay(1000);
//                 k = 0;
//             }
//             digitalWrite(r, LOW);
//             delay(1000);
//         }
//         delay(1000);
//     }
// }
