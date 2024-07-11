//
// Created by 75842 on 2024/1/26.
//
//接收MAC地址：0x74,0x4D,0xBD,0x7F,0x1F,0xC8
//1号呼叫机MAC地址：0xDC,0xDA,0x0C,0x11,0xE7,0xB0

//头文件声明
#include <esp_now.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
#include <stdlib.h>

//#define LED_OPEN 1

#ifdef LED_OPEN
#define LED_B num1
#define LED_W num2
/*定义任务句柄*/
TaskHandle_t TaskLedBlink;
#endif


//全局变量
esp_now_peer_info_t peerInfo1;

//TFT屏幕参数
TFT_eSPI tft = TFT_eSPI(); // Create TFT_eSPI object with pin connections


//变量定义
uint8_t TFT_NOW[] = {1};
uint8_t TFT_Rec = 1;
uint8_t TFT_Flag = 0;
uint8_t state = 0;
uint8_t send_key_state = 0;


//函数声明
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataReceived(const uint8_t *mac_addr, const uint8_t *data, int len);
void MAC_Ad();
void sendData(uint8_t data);
//函数声明
void tft_init();
void tft_drawCircle();
void station(int a);
void delate_circle(int a,int b);
void AGV_sta(int a);
void AGV_staDelate();
void Key_judge();
void LedBlink(void * pvParameters);
void Blink();


void tft_init(){
    // Initialize TFT screen
    tft.begin();
    tft.setRotation(0); // Set screen rotation direction (adjust as needed)
    // Clear the screen and fill with background color
    tft.fillScreen(TFT_BLACK);

}

// 画圆圈作为点位，表示站点位置
void tft_drawCircle(){
    /*---------------上排4个------------------*/
    tft.fillCircle(20,47,8,TFT_BLUE);//3
    tft.fillCircle(50,47,8,TFT_ORANGE);//4
    tft.fillCircle(80,47,8,TFT_YELLOW);//7
    tft.fillCircle(110,47,8,TFT_PINK);//8

    /*---------------下排4个------------------*/

    tft.fillCircle(20,142,8,TFT_GREEN);//5
    tft.fillCircle(50,142,8,TFT_SKYBLUE);//6
    tft.fillCircle(80,142,8,TFT_LIGHTGREY );//9
    tft.fillCircle(110,142,8,TFT_PURPLE);//10

    /*-----------------充电桩------------------*/
    tft.fillCircle(15,97,8,TFT_WHITE);//1

}

//机器人当前位置
void AGV_sta(int a){
    switch (a) {
        case 3:
            tft.fillCircle(20,67,8,TFT_RED);
            break;
        case 4:
            tft.fillCircle(50,67,8,TFT_RED);
            break;
        case 5:
            tft.fillCircle(20,122,8,TFT_RED);
            break;
        case 6:
            tft.fillCircle(50,122,8,TFT_RED);
            break;
        case 7:
            tft.fillCircle(80,67,8,TFT_RED);
            break;
        case 8:
            tft.fillCircle(110,67,8,TFT_RED);
            break;
        case 1:
            tft.fillCircle(35,97,8,TFT_RED);
            break;
        case 9:
            tft.fillCircle(80,122,8,TFT_RED);
            break;
        case 10:
            tft.fillCircle(110,122,8,TFT_RED);
        default:
            break;

    }
}
void AGV_staDelate(){
    tft.fillCircle(20,67,8,TFT_BLACK);
    tft.fillCircle(50,67,8,TFT_BLACK);
    tft.fillCircle(20,122,8,TFT_BLACK);
    tft.fillCircle(50,122,8,TFT_BLACK);
    tft.fillCircle(80,67,8,TFT_BLACK);
    tft.fillCircle(110,67,8,TFT_BLACK);
    tft.fillCircle(35,97,8,TFT_BLACK);
    tft.fillCircle(80,122,8,TFT_BLACK);
    tft.fillCircle(110,122,8,TFT_BLACK);

}



//站点位置闪烁
void station(int a){
    switch (a) {
        case 3:
            tft.fillCircle(20,47,8,TFT_BLACK);
            break;
        case 4:
            tft.fillCircle(50,47,8,TFT_BLACK);
            break;
        case 5:
            tft.fillCircle(20,142,8,TFT_BLACK);
            break;
        case 6:
            tft.fillCircle(50,142,8,TFT_BLACK);
            break;
        case 7:
            tft.fillCircle(80,47,8,TFT_BLACK);
            break;
        case 8:
            tft.fillCircle(110,47,8,TFT_BLACK);
            break;
//            充电位置
        case 1:
            tft.fillCircle(15,60,8,TFT_BLACK);
            break;
        case 9:
            tft.fillCircle(80,142,8,TFT_BLACK);
            break;
        case 10:
            tft.fillCircle(110,142,8,TFT_BLACK);
        default:
            break;

    }

}

//站点闪烁
void delate_circle(int agv,int hujiao){
    /***
     * 函数主要是agv位置点的闪烁和下一个点的闪烁
     */
    tft_drawCircle();
    AGV_sta(agv);
    Key_judge();
    delay(200);
    // 清除第一个圆
    station(hujiao);
    // 清除第二个圆,即终点位置闪烁
    AGV_staDelate();
    Key_judge();
    delay(200);
    // 其他操作
}


/*
 * LED闪烁任务函数
 * */
//void LedBlink(void * pvParameters){
//    while (1){
//        digitalWrite(LED_B_PIN,LOW);
//        delay(1000);
//        digitalWrite(LED_B_PIN,HIGH);
//        digitalWrite(LED_W_PIN,LOW);
//        delay(1000);
//        digitalWrite(LED_W_PIN,HIGH);
//    }
//}
///*
// * 创建LED闪烁任务核心
// *
// */
//void Blink(){
//    xTaskCreatePinnedToCore(
//            LedBlink,
//            "LedBlink",
//            1000,
//            NULL,
//            1,
//            &TaskLedBlink,
//            0
//    );
//}




// 回调函数，处理发送完成后的状态
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        Serial.println("Data sent successfully");
    } else {
        Serial.println("Error sending data");
    }
}
// 回调函数，处理接收到的数据
void OnDataReceived(const uint8_t *mac_addr, const uint8_t *data, int len) {
//    Serial.print("Received data from ");
//    for (int i = 0; i < 6; i++) {
//        Serial.print(mac_addr[i], HEX);
//        if (i < 5) {
//            Serial.print(":");
//        }
//    }
//    Serial.print(": ");
//    for (int i = 0; i < len; i++) {
//        Serial.print(data[i], HEX);
//        Serial.print(" ");
//    }
//    Serial.println();
//字符串转换
//    itoa(*data,TFT_Rec,10);

    if(*data == 1){
        TFT_Rec = *data;
        TFT_Flag = 0;
    }
    else
    {
        TFT_Flag = 1;
        TFT_NOW[0] = TFT_Rec;
        TFT_Rec = *data;
    }
}

// 接收设备MAC地址定义
void MAC_Ad(){
    // 添加接收设备1的MAC地址
    uint8_t receiverMac[] = {0xDC, 0xDA, 0x0C, 0x11, 0xE7, 0xB0};
    memcpy(peerInfo1.peer_addr, receiverMac, 6);
    peerInfo1.channel = 0;
    peerInfo1.encrypt = false;
    if (esp_now_add_peer(&peerInfo1) != ESP_OK) {
        Serial.println("Failed to add peer 1");
    }

}

void sendData(uint8_t data){
//    uint8_t data_send = data;
    esp_err_t result1 = esp_now_send(peerInfo1.peer_addr, &data, sizeof(data));
    if (result1 != ESP_OK) {
        Serial.println("Error sending data to receiver 1");
    }

}

//按键函数
void Key_judge(){
    state = digitalRead(1);
    if(state == 0)
    {
        TFT_Flag = 1;
        delay(20);
        while(state == 0)
        {
            state = digitalRead(1);
        }
        sendData(11);
    }

//送检按键判断
    send_key_state = digitalRead(2);
    if(send_key_state == 0)
    {
        delay(20);
        while(send_key_state == 0)
        {
            send_key_state = digitalRead(2);
        }
        sendData(5);
    }

}


void setup() {
    Serial.begin(115200);
    //按键引脚设置
    pinMode(1, INPUT_PULLUP);
    pinMode(2, INPUT_PULLUP);

    //屏幕初始化
    tft_init();
    // 初始化ESP-NOW
    WiFi.mode(WIFI_AP_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
    }
    MAC_Ad();
    // 注册接收回调函数
    esp_now_register_recv_cb(OnDataReceived);
    // 注册发送回调函数
    esp_now_register_send_cb(OnDataSent);

}

void loop() {

    Key_judge();
    //Serial.print(state);
    // wait for 3seconds to run the logic again
    delay(5);
    if(TFT_Flag == 0)
    {
        delate_circle(1,0);
    } else if(TFT_Flag == 1)
    {
        delate_circle(TFT_NOW[0],TFT_Rec);
    } else
        ;


}