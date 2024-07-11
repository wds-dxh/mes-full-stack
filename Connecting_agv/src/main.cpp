/*
 * @Author: wds-dxh wdsnpshy@163.com
 * @Date: 2024-07-10 18:48:59
 * @LastEditors: wds-dxh wdsnpshy@163.com
 * @LastEditTime: 2024-07-10 19:53:18
 * @FilePath: \Connecting_agv\src\main.cpp
 * @Description: 
 * 微信: 15310638214 
 * 邮箱：wdsnpshy@163.com 
 * Copyright (c) 2024 by ${wds-dxh}, All Rights Reserved. 
 */

#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>
#include "MQTT_EMQX.h"



/*
 * 数据存储变量
 */
char data_re[20];   //接收到的pc数据
bool status_arrive = false;


//定义mqtt和wifi
// WiFi和MQTT参数
const char *ssid = "wds";
const char *password = "wds666666";
const char *mqtt_broker = "vps.dxh-wds.top";
const char *topic = "agv_control_sub_topic";
const char *mqtt_username = "wds";
const char *mqtt_password = "wdsshy0320";
const int mqtt_port = 1883;
const char *topic_pub = "agv_esp32_pub_topic";
Mqtt_emqx mqtt(ssid, password, mqtt_broker, topic, mqtt_username, mqtt_password, mqtt_port, topic_pub);//实例化mqtt对象




void setup() {
    Serial.begin(115200);
    mqtt.connect();// 连接WiFi和MQTT
    mqtt.publish(topic_pub, "esp32 goes live"); // 发布消息
}


void loop() {

    if (Serial.available()>0) { // 如果电脑串口有数据
        memset(data_re,0,sizeof(data_re));
        status_arrive = true;
    }
    else{
    status_arrive = false;
    
    }

    mqtt.loop(); // 处理MQTT客户端循环

}






