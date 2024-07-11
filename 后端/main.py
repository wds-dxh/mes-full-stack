import datetime
from flask import Flask, request, jsonify
from connext_esp32 import MQTTClient
import json
import threading
from flask_cors import CORS
app = Flask(__name__)
CORS(app)  # 允许所有域名的所有请求

# MQTT配置
mqtt_host = 'vps.dxh-wds.top'
mqtt_port = 1883
mqtt_pub_topic = 'agv_control_sub_topic'
mqtt_username = 'wds'
mqtt_password = 'wdsshy0320'
mqtt_sub_topic = 'agv_esp32_pub_topic'

# 创建MQTT客户端
mqtt_client = MQTTClient(mqtt_host, mqtt_port, mqtt_username, mqtt_password, mqtt_pub_topic, mqtt_sub_topic)

# 存储最新的AGV状态
agv_status = {
    "position": 1,
    "status": "离线",
    "battery": 100,
    "speed": 100,  # cm/s
    "last_update": datetime.datetime.now().isoformat()
}

# 修改MQTT客户端的on_message回调
def on_message(client, userdata, msg):
    global agv_status
    try:
        message = msg.payload.decode()
        #message为json数据
        message = json.loads(message)
        agv_status["status"] = message["status"]
        # agv_status["status"] = message
        agv_status["last_update"] = datetime.datetime.now().isoformat()
        print(f"Received message: {message}")
    except Exception as e:
        print(f"Error processing message: {e}")

mqtt_client.client.on_message = on_message

# 启动MQTT客户端
def start_mqtt_client():
    mqtt_client.start()

# 在单独的线程中启动MQTT客户端
mqtt_thread = threading.Thread(target=start_mqtt_client)
mqtt_thread.start()

@app.route('/agv/control', methods=['POST'])
def agv_control():
    data = request.get_json()
    position = data.get('position')
    
    if position is None:
        return jsonify({"error": "Position is required"}), 400
    
    # 发布MQTT消息
    mqtt_client.publish_message(position)
    #更新AGV状态
    agv_status["position"] = position

    return jsonify({"message": f"AGV position set to {position}"}), 200

@app.route('/agv/status', methods=['POST'])
def get_agv_status():
    data = request.get_json()
    detail = data.get('detail', 'basic') if data else 'basic'
    
    if detail == 'full':
        return jsonify(agv_status)
    else:
        basic_status = {
            "position": agv_status["position"],
            "status": agv_status["status"]
        }
        return jsonify(basic_status)

if __name__ == '__main__':
    app.run(debug=True)