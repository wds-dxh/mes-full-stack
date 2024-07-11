#include "MQTT_EMQX.h"
#include <ArduinoJson.h>





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
extern bool status_arrive;



// 初始化静态成员
Mqtt_emqx* Mqtt_emqx::instance = nullptr;

Mqtt_emqx::Mqtt_emqx(const char *ssid, const char *password, const char *mqtt_broker, const char *topic, const char *mqtt_username, const char *mqtt_password, const int mqtt_port, const char *topic_pub)
    : ssid(ssid), password(password), mqtt_broker(mqtt_broker), topic(topic), mqtt_username(mqtt_username), mqtt_password(mqtt_password), mqtt_port(mqtt_port), client(espClient), incomingMessage(""), messagePending(false), topic_pub(topic_pub) {
    instance = this;  // 在构造函数中设置全局实例指针
}//把构造函数的参数传递给类的私有成员变量，括号前的是类的私有成员变量，括号内的是构造函数的参数

void Mqtt_emqx::callback(char *topic, byte *payload, unsigned int length) {
    if (instance) {
        instance->incomingMessage = "";
        for (unsigned int i = 0; i < length; i++) {
            instance->incomingMessage += (char)payload[i];
        }
        instance->messagePending = true;        // 设置消息标志，可以开始执行消息处理
    }
}

bool Mqtt_emqx::connect() {
    // 连接WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }

    Serial.println("Connected to the WiFi network");
    Serial.print("IPv4: ");
    Serial.println(WiFi.localIP());
    Serial.print("IPv6: ");
    Serial.println(WiFi.localIPv6());

    // 连接MQTT
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(Mqtt_emqx::callback);

    while (!client.connected()) {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());

        Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Public emqx mqtt broker connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }

    client.subscribe(topic);
    return true;
}

void Mqtt_emqx::subscribe(const char *topic) {
    client.subscribe(topic);
}

void Mqtt_emqx::publish(const char *topic, const char *data) {
    client.publish(topic, data);
}

// void Mqtt_emqx::send_task(char *data)
// {
//   if (status_arrive)
//   {
//     Serial.write(data,sizeof(data));
//     delay(200);
//   }
// }


void Mqtt_emqx::loop() {
    client.loop();

    if (messagePending) {
        // Serial.print("Incoming message: ");
        // Serial.println(incomingMessage);
        this->publish(topic_pub, incomingMessage.c_str());  // 发布消息, 用于调试

        // 解析 JSON 消息
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, incomingMessage);
        if (!error) {
            const char* position = doc["position"];
            Serial.print("Position: ");
            Serial.println(position);
            
            // 发送任务
            if (strcmp(position, "1") == 0) {
                Serial.write(data1, sizeof(data1));
                delay(200);
                Serial.write(data1, sizeof(data1));
                delay(200);

            } else if (strcmp(position, "2") == 0) {
                Serial.write(data2, sizeof(data2));
                delay(200);
                Serial.write(data2, sizeof(data2));
                delay(200);

            } else if (strcmp(position, "3") == 0) {
                Serial.write(data3, sizeof(data3));
                delay(200);
                Serial.write(data3, sizeof(data3));
                delay(200);

            } else if (strcmp(position, "4") == 0) {
                Serial.write(data4, sizeof(data4));
                delay(200);

            } else if (strcmp(position, "5") == 0) {
                Serial.write(data5, sizeof(data5));
                delay(200);
                Serial.write(data5, sizeof(data5));
                delay(200);

            } else if (strcmp(position, "6") == 0) {
                Serial.write(data8, sizeof(data8));
                delay(200);
                Serial.write(data8, sizeof(data8));
                delay(200);

            } else if (strcmp(position, "7") == 0) {
                Serial.write(data9, sizeof(data9));
                delay(200);
                Serial.write(data9, sizeof(data9));
                delay(200);

            } else if (strcmp(position, "8") == 0) {
                Serial.write(data10, sizeof(data10));
                delay(200);
                Serial.write(data10, sizeof(data10));
                delay(200);
                
            } else if (strcmp(position, "9") == 0) {
                Serial.write(cancel_task, sizeof(cancel_task));
                delay(200);
                Serial.write(cancel_task, sizeof(cancel_task));
                delay(200);

            } else if (strcmp(position, "10") == 0) {
                Serial.write(back, sizeof(back));
                delay(200);
                Serial.write(back, sizeof(back));
                delay(200);

            } else if (strcmp(position, "11") == 0) {
                Serial.write(zhuanwan, sizeof(zhuanwan));
                delay(200);
                Serial.write(zhuanwan, sizeof(zhuanwan));
                delay(200);

            } else {
                this->publish(topic_pub, "Invalid position");
            }
        }

        messagePending = false; // 重置消息标志
    }


}

