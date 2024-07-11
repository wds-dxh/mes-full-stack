'''
Author: wds-dxh wdsnpshy@163.com
Date: 2024-07-11 11:00:35
LastEditors: wds-dxh wdsnpshy@163.com
LastEditTime: 2024-07-11 11:26:55
FilePath: \Data_persistence\Into_smart_farm.py
Description: 
微信: 15310638214 
邮箱：wdsnpshy@163.com 
Copyright (c) 2024 by ${wds-dxh}, All Rights Reserved. 
'''
from pymysql import Connection

con = None
try:
    # 连接数据库
    con = Connection(
            host='vps.dxh-wds.top',
            user='root',
            password='wdsshy0320',
            database='smart_factory',  # 指定数据库
        )
    print("连接成功")
    print(con)

    # 插入数据的 SQL 语句，包含 temperature、humidness、illumination 和 timestamp 字段
    sql = """
    INSERT INTO smart_factory_status(temperature, humidness, illumination, timestamp)
    VALUES(33, 66, 555, CURRENT_TIMESTAMP)
    """

    # 使用游标对象执行sql语句并获取结果
    with con.cursor() as cursor:
        cursor.execute(sql)
        # 插入数据后需要提交
        con.commit()
        print("插入成功")


except Exception as e:
    print("连接失败")
    print(e)
finally:
    con.close()
