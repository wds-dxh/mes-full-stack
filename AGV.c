//
// Created by 75842 on 2024/3/22.
//

#include <esp_now.h>
#include <WiFi.h>
#include <esp_system.h>
#include <Arduino.h>
#define LED_OPEN
#define HORN_OPEN
/*
 * LED灯的引脚重定义
 */
#ifdef LED_OPEN
#define LED_W_PIN 42
#define LED_B_PIN 41
#endif


/*
 * 功放喇叭引脚重定义
 */
#ifdef HORN_OPEN
#define HORN_PIN 7
#endif


//全局变量---->呼叫机的MAC地址全局变量
esp_now_peer_info_t peerInfo1;
esp_now_peer_info_t peerInfo2;
esp_now_peer_info_t peerInfo3;
esp_now_peer_info_t peerInfo4;
esp_now_peer_info_t peerInfo5;
esp_now_peer_info_t peerInfo6;
esp_now_peer_info_t peerInfo7;
esp_now_peer_info_t peerInfo8;

/*定义任务句柄*/
TaskHandle_t TaskLedBlink;

//点位串口数据定义
char data1[] = {0xAA,0xBB,0x09,0x02,0x03,0x00,0x00,0x00,0x01,0x8B};//1,对应3号呼叫机器
char data2[] = {0xAA,0xBB,0x09,0x02,0x04,0x00,0x00,0x00,0x01,0x8A};//2，对应4号呼叫机器
char data3[] = {0xAA,0xBB,0x09,0x02,0x05,0x00,0x00,0x00,0x01,0x89};//3，对应5号呼叫机器
char data4[] = {0xAA,0xBB,0x09,0x02,0x06,0x00,0x00,0x00,0x01,0x88};//4，对应6号呼叫机器
char data5[] = {0xAA,0xBB,0x09,0x02,0x07,0x00,0x00,0x00,0x01,0x87};//5，对应7号呼叫机器
// char data5[] = {0xAA,0xBB,0x09,0x02,0x08,0x07,0x00,0x00,0x01,0x7F};//5号，待定
char back[]  = {0xAA,0xBA,0x09,0x02,0x00,0x00,0x01,0x00,0x00,0x8F};//回充电
char zhuanwan[] = {0xAA,0xBC,0x09,0x02,0x01,0x00,0x00,0x00,0x01,0x8c};//转弯
char data8[]   = {0xAA,0xBB,0x09,0x02,0x08,0x00,0x00,0x00,0x01,0x86};// 6号点，对应8号任务
char data9[] = {0xAA,0xBB,0x09,0x02,0x09,0x00,0x00,0x00,0x01,0x85};//9号任务
char data10[] = {0xAA,0xBB,0x09,0x02,0x0A,0x00,0x00,0x00,0x01,0x84};//10号任务
char cancel_task[]   = {0xAA,0xAE,0x09,0x00,0x01,0x00,0xFF,0x00,0x00,0x9E};//取消任务

/*
 * 标志位变量定义
 */
uint8_t flag = 0; //送检按钮按下标志位
uint8_t flag_a = 0;
uint8_t statue = 1;
uint8_t flag_b = 1;
uint8_t flag_c = 0;//三号坐标室标志位
uint8_t front_or_behind_5 = 1;
uint8_t Is_canBeCalled = 1;
uint8_t last_data;
uint8_t flag_8 = 0;
uint8_t flag_charge = 0;
/*
 * 数据存储变量
 */
uint8_t called_buffer[][2]={0};
uint8_t buffer_cnt=0;
uint8_t data_buffer[4];
char data_re[20];


/*
 * 函数声明
 */
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataReceived(const uint8_t *mac_addr, const uint8_t *data, int len);
void MAC_Add();
void sendData(uint8_t data);
void send_task();
void LedBlink(void * pvParameters);
void Blink();

/*
 * LED闪烁任务函数
 * */
void LedBlink(void * pvParameters){
    while (1){
        digitalWrite(LED_B_PIN,LOW);
        delay(500);
        digitalWrite(LED_B_PIN,HIGH);
        digitalWrite(LED_W_PIN,LOW);
        delay(500);
        digitalWrite(LED_W_PIN,HIGH);
    }
}

/*
 * 创建LED闪烁任务核心
 *
 */
void Blink(){
    xTaskCreatePinnedToCore(
            LedBlink,
            "LedBlink",
            1000,
            NULL,
            1,
            &TaskLedBlink,
            0
    );
}



/*
 * 回调函数，处理发送完成后的状态
 */
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        Serial.println("Data sent successfully");
    } else {
        Serial.println("Error sending data");
    }
}


/*
 * 回调函数，处理接收到的数据
 */
void OnDataReceived(const uint8_t *mac_addr, const uint8_t *data, int len) {
        flag_8 = 1;
        if(*data == 2 && Is_canBeCalled == 0 && front_or_behind_5 == 1)// 第二次呼叫 需要等待之前的执行完毕 将接收到的数据存入buffer，并且再后面清空
        {
            //存入buffer
            buffer_cnt += 1;
            called_buffer[buffer_cnt][0]=2; //将cnt++
            flag_b = 1;
            Serial.println("222okoko");
        }else if(*data==1 && Is_canBeCalled==0 && front_or_behind_5==1)// 第二次呼叫 需要等待之前的执行完毕 将接收到的数据存入buffer，并且再后面清空
        {
            //存入buffer
            buffer_cnt += 1;
            called_buffer[buffer_cnt][0]=1; //将cnt++
            flag_b = 1;
            Serial.println("11okoko");
        }else if(*data==3 &&Is_canBeCalled==0 &&front_or_behind_5==1)// 第二次呼叫 需要等待之前的执行完毕 将接收到的数据存入buffer，并且再后面清空
        {
            //存入buffer
            buffer_cnt += 1;
            called_buffer[buffer_cnt][0]=3;
            flag_b = 1;
            Serial.println("333okoko");
        }else if(*data==4 &&Is_canBeCalled==0 &&front_or_behind_5==1)// 第二次呼叫 需要等待之前的执行完毕 将接收到的数据存入buffer，并且再后面清空
        {
            //存入buffer
            buffer_cnt += 1;
            called_buffer[buffer_cnt][0]=4; //将cnt++
            flag_b = 1;
            Serial.println("444okoko");
        }else if(*data==6 &&Is_canBeCalled==0 &&front_or_behind_5==1)// 第二次呼叫 需要等待之前的执行完毕 将接收到的数据存入buffer，并且再后面清空
        {
            //存入buffer
            buffer_cnt += 1;
            called_buffer[buffer_cnt][0]=5; //将cnt++
            flag_b = 1;
            Serial.println("444okoko");
        }else if(*data==9 &&Is_canBeCalled==0 &&front_or_behind_5==1)// 第二次呼叫 需要等待之前的执行完毕 将接收到的数据存入buffer，并且再后面清空
        {
            //存入buffer
            buffer_cnt += 1;
            called_buffer[buffer_cnt][0]=7; //将cnt++
            flag_b = 1;
            Serial.println("444okoko");
        }
        else if(*data==8 &&Is_canBeCalled==0 &&front_or_behind_5==1)//三号坐标室
        {
            //存入buffer
            buffer_cnt += 1;
            called_buffer[buffer_cnt][0]=6; //将cnt++
            flag_b = 1;
            Serial.println("555okoko");
        } else
        {
            ;
        }


        if(*data == 1 && Is_canBeCalled == 1 && front_or_behind_5 == 1)//HUJIAO 假如是第一次呼叫，需要等该点按下送检，其他的呼叫才能被执行
        {
            flag_charge = 0;
            sendData(3);
            delay(100);
            flag_b = 1;
            //先发送取消任务
            Serial.write(cancel_task,sizeof(cancel_task));
            delay(200);
            Serial.write(data1,sizeof(data1)); //前往一号点
            Is_canBeCalled=0;
            Serial.println("f1111");
        }

        else if(*data == 2 && Is_canBeCalled == 1 && front_or_behind_5 == 1)//任务点2 第一次呼叫
        {
            flag_charge = 0;

            sendData(4);
            delay(100);

            Serial.write(cancel_task, sizeof(cancel_task));
            delay(200);
            Serial.write(data2, sizeof(data2));
            Is_canBeCalled=0;
            flag_b = 1;
            Serial.println("f2222");
        }
        else if(*data==3 && Is_canBeCalled == 1 && front_or_behind_5 == 1)//任务点3 第一次呼叫
        {
            flag_charge = 0;

            sendData(5);
            delay(100);

            Serial.write(cancel_task, sizeof(cancel_task));
            delay(200);
            Serial.write(data3, sizeof(data3));
            Is_canBeCalled=0;
            flag_b = 1;
            Serial.println("f3333");
        }
        else if(*data==4 && Is_canBeCalled == 1 && front_or_behind_5 == 1)//任务点4 第一次呼叫
        {
            flag_charge = 0;

            sendData(6);
            delay(100);

            Serial.write(cancel_task,sizeof(cancel_task)); //取消
            delay(500);
            Serial.write(data4,sizeof(data4)); //
            Is_canBeCalled=0;
            flag_b = 1;
            //Serial.println("f4444");
        } else if(*data== 6 && Is_canBeCalled == 1 && front_or_behind_5 == 1){//任务点5第一次呼叫
            flag_charge = 0;

            sendData(7);
            delay(100);
            Serial.write(cancel_task,sizeof(cancel_task)); //取消
            delay(200);
            Serial.write(data5,sizeof(data5)); //
            Is_canBeCalled=0;
            flag_b = 1;
            //Serial.println("f5555");
        }else if(*data == 8 && Is_canBeCalled == 1 && front_or_behind_5 == 1){//任务点6第一次呼叫
            flag_charge = 0;

            sendData(8);
            delay(100);
            Serial.write(cancel_task,sizeof(cancel_task)); //取消
            delay(200);
            Serial.write(data8,sizeof(data8)); //
            Is_canBeCalled=0;
            flag_b = 1;
            //Serial.println("f5555");
        }

        //三号坐标室按键任务
        else if(*data == 1 && flag_c == 1){
            flag_charge = 1;
            delay(200);
            Serial.write(data1,sizeof(data1));
            flag_c = 0;
        }
        else if(*data == 2 && flag_c == 1){
            flag_charge = 1;
            delay(200);
            Serial.write(data2,sizeof(data2));
            flag_c = 0;
        }
        else if(*data == 3 && flag_c == 1){
            flag_charge = 1;
            delay(200);
            Serial.write(data3,sizeof(data3));
            flag_c = 0;
        }
        else if(*data == 4 && flag_c == 1){
            flag_charge = 1;
            delay(200);
            Serial.write(data4,sizeof(data4));
            flag_c = 0;
        }
        else if(*data == 6 && flag_c == 1){
            flag_charge = 1;
            delay(200);
            Serial.write(data5,sizeof(data5));
            flag_c = 0;
        }



        else if(*data==5 && statue == 1)
        {
            // Serial.write(back,sizeof(back));
            if(buffer_cnt == 0 && flag_b == 1){
                if(flag_charge == 1){
                    Is_canBeCalled = 1;
                    flag = 0;
                    flag_b = 1;
                    flag_a = 0;
                    flag_c = 0;
                    buffer_cnt=0;
                    flag_charge = 0;
                    front_or_behind_5 = 1;
                    // delay(200);
                    sendData(1);
                    delay(200);
                    Serial.write(back,sizeof(back));
                }else{
                    flag_b = 0;
                    sendData(8);
                    delay(200);
                    Serial.write(data9,sizeof(data9));//送检
                    flag = 0;
                    flag_a = 1;
                    Is_canBeCalled=1;
                }

            }else if(buffer_cnt != 0){
                flag = 1;//
                flag_a = 0;
                flag_b = 0;
                Serial.println("buff!=0");
            }
            statue = 0;
        }

        else if(*data==7)//充电
        {
//            digitalWrite(8,LOW);
            Is_canBeCalled = 1;
            flag = 0;
            flag_b = 1;
            flag_a = 0;
            flag_c = 0;
            buffer_cnt=0;
            front_or_behind_5 = 1;
            // delay(200);
            sendData(1);
            delay(200);
            Serial.write(back,sizeof(back));
        }else
            Serial.print("hhhh");


}


/*
 * 此处按格式添加接收设备MAC地址
 */
void MAC_Add(){
    // 添加接收设备1的MAC地址
    uint8_t receiverMac1[] = {0xDC, 0xDA, 0x0C, 0x11, 0xE7, 0xB0};
    memcpy(peerInfo1.peer_addr, receiverMac1, 6);
    peerInfo1.channel = 0;
    peerInfo1.encrypt = false;
    if (esp_now_add_peer(&peerInfo1) != ESP_OK) {
        Serial.println("Failed to add peer 1");
    }
    // 添加接收设备2的MAC地址
    uint8_t receiverMac2[] = {0xDC, 0xDA, 0x0C, 0x11, 0xE7, 0xA8};
    memcpy(peerInfo2.peer_addr, receiverMac2, 6);
    peerInfo2.channel = 0;
    peerInfo2.encrypt = false;
    if (esp_now_add_peer(&peerInfo2) != ESP_OK) {
        Serial.println("Failed to add peer 2");
    }
    // 添加接收设备3的MAC地址
    uint8_t receiverMac3[] = {0x74, 0x4D, 0xBD, 0x7F, 0x2B, 0xB8};
    memcpy(peerInfo3.peer_addr, receiverMac3, 6);
    peerInfo3.channel = 0;
    peerInfo3.encrypt = false;
    if (esp_now_add_peer(&peerInfo3) != ESP_OK) {
        Serial.println("Failed to add peer 3");
    }
    // 添加接收设备4的MAC地址
    uint8_t receiverMac4[] = {0x74, 0x4D, 0xBD, 0x7F, 0x2F, 0x58};
    memcpy(peerInfo4.peer_addr, receiverMac4, 6);
    peerInfo4.channel = 0;
    peerInfo4.encrypt = false;
    if (esp_now_add_peer(&peerInfo4) != ESP_OK) {
        Serial.println("Failed to add peer 4");
    }

}


/*
 * 向指定MAC地址设备发送data数据
 */
void sendData(uint8_t data){
    esp_err_t result1 = esp_now_send(peerInfo1.peer_addr, &data, sizeof(data));
    if (result1 != ESP_OK) {
        Serial.println("Error sending data to receiver 1");
    }

    esp_err_t result2 = esp_now_send(peerInfo2.peer_addr, &data, sizeof(data));
    if (result2 != ESP_OK) {
        Serial.println("Error sending data to receiver 1");
    }

    esp_err_t result3 = esp_now_send(peerInfo3.peer_addr, &data, sizeof(data));
    if (result3 != ESP_OK) {
        Serial.println("Error sending data to receiver 3");
    }

    esp_err_t result4 = esp_now_send(peerInfo4.peer_addr, &data, sizeof(data));
    if (result4 != ESP_OK) {
        Serial.println("Error sending data to receiver 4");
    }

    esp_err_t result5 = esp_now_send(peerInfo5.peer_addr, &data, sizeof(data));
    if (result5 != ESP_OK) {
        Serial.println("Error sending data to receiver 1");
    }

    esp_err_t result6 = esp_now_send(peerInfo6.peer_addr, &data, sizeof(data));
    if (result6 != ESP_OK) {
        Serial.println("Error sending data to receiver 6");
    }

    esp_err_t result7 = esp_now_send(peerInfo7.peer_addr, &data, sizeof(data));
    if (result7 != ESP_OK) {
        Serial.println("Error sending data to receiver 7");
    }

    esp_err_t result8 = esp_now_send(peerInfo8.peer_addr, &data, sizeof(data));
    if (result8 != ESP_OK) {
        Serial.println("Error sending data to receiver 8");
    }

}


void setup() {
    Serial.begin(115200);
    // 初始化ESP-NOW
    WiFi.mode(WIFI_AP_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
    }
    MAC_Add();

    //LED灯引脚配置
    pinMode(LED_W_PIN,OUTPUT);
    pinMode(LED_B_PIN,OUTPUT);
    Blink();
//    发送初始位置
    sendData(1);
//    esp_now_set_wake_window(10000);  //这是 一个wake up窗口函数
    // 注册接收回调函数
    esp_now_register_recv_cb(OnDataReceived);
    // 注册发送回调函数
    esp_now_register_send_cb(OnDataSent);

}


/*
 * 处理串口接收到的数据
 */
void loop() {
    if (Serial.available()>0) { // 如果电脑串口有数据
        statue = 1;
        memset(data_re,0,sizeof(data_re));
        byte rcmark=0;
        while(Serial.available()>0)
        {
            data_re[rcmark]=char(Serial.read());
            delay(2);
            rcmark++;
        }
    }



    if((data_re[4] == 0x09) && flag_a == 1) //到达判断点  直接送检
    {
        statue = 1;
        sendData(8);
        front_or_behind_5 = 0;
        flag_b = 1;
        flag_a = 0;
        flag = 0;
        delay(200);
        Serial.write(data8, sizeof(data8)); //直接送往终点
    }else if((data_re[4] == 0x03) && flag == 1) //如果到达了1号点 flag==1是buffercnt==0时赋值
    {
        statue = 1;
        sendData(3);
        delay(100);
        //Serial.println(called_buffer[0][0]);
        called_buffer[0][0] = called_buffer[buffer_cnt][0];//将最后一位拿过来
//        delay(100);
        buffer_cnt--;
        send_task();
        flag = 0;
        flag_b = 1;
        called_buffer[0][0] = 0;
        Serial.println("receive3");
    }else if(data_re[4] == 0x04 && flag == 1)
    {
        statue = 1;
        sendData(4);
        delay(100);
        called_buffer[0][0]=called_buffer[buffer_cnt][0];//将最后一位拿过来
//        delay(100);
        buffer_cnt--;
        send_task();
        called_buffer[0][0] = 0;
        flag=0;
        flag_b = 1;
        Serial.println("receive4");


    }else if(data_re[4]==0x05 && flag == 1) //如果到达了
    {
        statue = 1;
        sendData(5);
        delay(100);

        called_buffer[0][0]=called_buffer[buffer_cnt][0];//将最后一位拿过来
//        delay(100);
        buffer_cnt--;
        send_task();
        called_buffer[0][0] = 0;
        flag=0;
        flag_b = 1;
        Serial.println("receive5");
    }else if(data_re[4]==0x06 && flag ==1)
    {
        statue = 1;
        sendData(6);
        delay(100);


        called_buffer[0][0]=called_buffer[buffer_cnt][0];//将最后一位拿过来
//        delay(100);
        buffer_cnt--;
        send_task();
        called_buffer[0][0] = 0;
        flag=0;
        flag_b = 1;
        Serial.println("receive6");
    }else if(data_re[4]==0x07 && flag ==1)
    {
        statue = 1;
        sendData(7);
        delay(100);
        called_buffer[0][0]=called_buffer[buffer_cnt][0];//将最后一位拿过来
//        delay(100);
        buffer_cnt--;
        send_task();
        called_buffer[0][0] = 0;
        flag=0;
        flag_b = 1;
        Serial.println("receive9");
    }else if(data_re[4] == 0x08 && flag_8 == 1){
//        sendData(9);
        flag_c = 1;
        statue = 1;
        flag_8 = 0;
        flag_charge = 0;

    }


}


/*
 * 对called_buffer里的数据进行判断，并执行对应的操作
 */
void send_task()
{
    statue = 1;
    flag_charge = 0;

    Serial.write(cancel_task,sizeof(cancel_task));
    if(called_buffer[0][0] == 1 || called_buffer[0][0]=='1'||called_buffer[0][0]==0x31||called_buffer[0][0]==0x01){
        sendData(3);
        Serial.write(data1,sizeof(data1));
    }
    else if(called_buffer[0][0]==2||called_buffer[0][0]=='2'||called_buffer[0][0]==0x32||called_buffer[0][0]==0x02){
        sendData(4);
        Serial.write(data2,sizeof(data2));
    }
    else if(called_buffer[0][0]==3||called_buffer[0][0]=='3'||called_buffer[0][0]==0x33||called_buffer[0][0]==0x03){
        sendData(5);
        Serial.write(data3,sizeof(data3));
    }
    else if(called_buffer[0][0]==4||called_buffer[0][0]=='4'||called_buffer[0][0]==0x34||called_buffer[0][0]==0x04){
        sendData(6);
        Serial.write(data4,sizeof(data4));
    }else if(called_buffer[0][0]==5||called_buffer[0][0]=='5'||called_buffer[0][0]==0x35||called_buffer[0][0]==0x05){
        sendData(8);
        Serial.write(data5,sizeof(data5));
    }else if(called_buffer[0][0]==6||called_buffer[0][0]=='6'||called_buffer[0][0]==0x36||called_buffer[0][0]==0x06){
        sendData(7);
        Serial.write(data8,sizeof(data8));
    }
//    else if(called_buffer[0][0]==9||called_buffer[0][0]=='9'||called_buffer[0][0]==0x39||called_buffer[0][0]==0x09){
//        SerialSend(data5);
//    }else if(called_buffer[0][0]==10||called_buffer[0][0]=='10'||called_buffer[0][0]==0x3A||called_buffer[0][0]==0x0A){
//        SerialSend(data6);
//    }
}


