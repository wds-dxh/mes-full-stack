import pymysql
from flask import Flask, request, jsonify
from flask_cors import CORS
import datetime
import json
import threading
from connext_esp32 import MQTTClient

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
        # message为json数据
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

@app.route('/api/login', methods=['POST'])
def login():
    data = request.get_json()
    username = data.get('username')
    password = data.get('password')

    con = get_db_connection()
    if not con:
        return jsonify({"error": "数据库连接失败"}), 500

    try:
        with con.cursor() as cursor:
            sql = "SELECT * FROM Smart_factory_login WHERE username=%s AND password=%s"
            cursor.execute(sql, (username, password))
            result = cursor.fetchone()

        if result:
            return jsonify(success=True, redirect_url="/Dashboard")
        else:
            return jsonify(success=False), 401
    except Exception as e:
        print("查询失败")
        print(e)
        return jsonify({"error": "查询失败"}), 500
    finally:
        con.close()

@app.route('/agv/control', methods=['POST'])
def agv_control():
    data = request.get_json()
    position = data.get('position')
    
    if position is None:
        return jsonify({"error": "Position is required"}), 400
    
    # 发布MQTT消息
    mqtt_client.publish_message(position)
    # 更新AGV状态
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

@app.route('/workshop/status', methods=['POST'])
def get_workshop_status():
    try:
        data = request.get_json()
        limit = data.get('limit', 10)  # 默认查询最近的 100 条记录

        # 连接数据库
        con = pymysql.connect(
            host='vps.dxh-wds.top',
            user='root',
            password='wdsshy0320',
            database='smart_factory',  # 指定数据库
        )
        print("数据库连接成功")

        # 查询数据的 SQL 语句
        sql = """
        SELECT temperature, humidness, illumination, timestamp
        FROM Smart_factory_status
        ORDER BY timestamp DESC
        LIMIT %s
        """
        # 使用游标对象执行 SQL 语句并获取结果
        with con.cursor() as cursor:
            cursor.execute(sql, (limit,))
            result = cursor.fetchall()
            data = []
            for row in result:
                data.append({
                    "temperature": row[0],
                    "humidness": row[1],
                    "illumination": row[2],
                    "timestamp": row[3].strftime('%Y-%m-%d %H:%M:%S')
                })
            return jsonify(data)
    except Exception as e:
        print("数据库连接失败")
        print(e)
        return jsonify({"error": "Database connection failed"}), 500
    finally:
        if con:
            con.close()

def get_db_connection():
    try:
        con = pymysql.connect(
            host='vps.dxh-wds.top',
            user='root',
            password='wdsshy0320',
            database='smart_factory',  # 指定数据库
        )
        return con
    except Exception as e:
        print("数据库连接失败")
        print(e)
        return None

if __name__ == '__main__':
    app.run(debug=True)
