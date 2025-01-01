import ultralytics
from ultralytics import YOLO
from ultralytics.utils.checks import check_yolo

def run_checks():
    # TODO maybe some other things
    check_yolo()


def train():
    model = YOLO("yolo11n.pt")
    results = model.train(data="coco8.yaml", epochs=5, imgsz=640)
