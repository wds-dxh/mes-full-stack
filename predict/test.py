'''
Author: wds-dxh wdsnpshy@163.com
Date: 2024-07-10 21:43:33
LastEditors: wds-dxh wdsnpshy@163.com
LastEditTime: 2024-07-10 22:31:39
FilePath: \predict\test.py
Description: 
微信: 15310638214 
邮箱：wdsnpshy@163.com 
Copyright (c) 2024 by ${wds-dxh}, All Rights Reserved. 
'''

import time
import os
import cv2
os.environ['YOLO_VERBOSE'] = str(False)
from ultralytics import YOLO



model = YOLO('best.pt')
video_stream_url = 1
cap = cv2.VideoCapture(video_stream_url)
start_time = time.time()

cout = 0
if __name__ == '__main__':
    while cap.isOpened():
        cout += 1
        frame = cv2.imread("non_fire.33.png")
        if frame is not None:
            if cout % 3 == 0:
                results = model.predict(frame, conf=0.4, imgsz=(640, 480), max_det=3, save=True)
                annotated_frame = results[0].plot()
                cls = results[0].names
                probs = results[0].probs
                

                print(cls[probs.top5[0]])
                print(probs.top5[0])
                

                cv2.imshow("hexapod", annotated_frame)
                fps = 1.0 / (time.time() - start_time)
                start_time = time.time()
                # cv2.putText(annotated_frame, f"{fps:.1f} FPS", (0, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
                
        if cv2.waitKey(1) & 0xFF == ord("q"):
            break
        

    cap.release()
    cv2.destroyAllWindows()

