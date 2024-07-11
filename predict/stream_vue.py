import os
import cv2
import time
from flask import Flask, Response, render_template
from ultralytics import YOLO

os.environ['YOLO_VERBOSE'] = str(False)

class FireDetection:
    def __init__(self, model_path='best.pt'):
        self.model = YOLO(model_path)
    
    def process(self, frame):
        results = self.model.predict(frame, conf=0.4, imgsz=(640, 480), max_det=3, save=True)
        annotated_frame = results[0].plot()
        cls = results[0].names
        probs = results[0].probs
        top_class = cls[probs.top5[0]]
        top_prob = probs.top5[0]
        return annotated_frame, top_class, top_prob

app = Flask(__name__)

@app.route('/')
def index():
    # 前端页面可以简单显示视频流
    return render_template('index.html')

def generate():
    # cap = cv2.VideoCapture("test.mp4")
    cap = cv2.VideoCapture(0)
    fd = FireDetection()
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        annotated_frame, top_class, top_prob = fd.process(frame)
        # if top_class == 'train_fire':
        _, buffer = cv2.imencode('.jpg', annotated_frame)
        frame = buffer.tobytes()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

    cap.release()

@app.route('/video_feed')
def video_feed():
    return Response(generate(),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5001, debug=True)