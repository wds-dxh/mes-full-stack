import time
import os
import cv2
os.environ['YOLO_VERBOSE'] = str(False)
from ultralytics import YOLO


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
    
if __name__ == "__main__":
    cap = cv2.VideoCapture("test.mp4")
    while True:
        _, frame = cap.read()
        if frame is not None:
            fd = FireDetection()
            annotated_frame, top_class, top_prob = fd.process(frame)
            print(top_class)
            cv2.imshow("hexapod", annotated_frame)
            if cv2.waitKey(1) & 0xFF == ord("q"):
                break
    cap.release()
    cv2.destroyAllWindows()