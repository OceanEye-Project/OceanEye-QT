import ultralytics
from ultralytics import YOLO

import yaml
import json
import random

def run_checks():
    # TODO maybe some other things
    ultralytics.checks()


def train():
    COCO_JSON = '/kaggle/input/urchin/dataset.json'
    ANNOTATIONS_DIR = 'datasets'
    VALIDATE_SPLIT = 0.15
    TEST_SPLIT = 0
    TRAIN_SPLIT = 1 - TEST_SPLIT - VALIDATE_SPLIT
    BACKGROUND_PERCENT = 0.1

    # YOLO config file
    coco_data = None
    with open(COCO_JSON, "r") as file:
        coco_data = json.load(file)

    yolo_data = {f
        "path": "",
        "train": "train.txt",
        "test": "test.txt",
        "val": "val.txt",
        "names": dict((int(key["id"]) - 1, key["name"]) for key in coco_data["categories"])
    }

    with open('data.yaml', 'w') as outfile:
        yaml.dump(yolo_data, outfile, default_flow_style=False)

    # Extract data from COCO JSON
    data = {}
    for image in coco_data["images"]:
        data[image["id"]] = {
            "name": ".".join(image["file_name"].split(".")[:-1]),
            "width": image["width"],
            "height": image["height"],
            "annotations": []
        }

    for annotation in coco_data["annotations"]:
        image = data[annotation["image_id"]]
        box = annotation["bbox"]
        box[0] /= image["width"]
        box[1] /= image["height"]
        box[2] /= image["width"]
        box[3] /= image["height"]

        box[0] += box[2] / 2
        box[1] += box[3] / 2

        image["annotations"].append({
            "type": int(annotation["category_id"]) - 1,
            "box": box
        })

    # Write annotation files in YOLO format
    for image in data.values():
        with open(f"{ANNOTATIONS_DIR}/labels/{image['name']}.txt", "w") as file:
            file.write("\n".join(
                f"{a['type']} {a['box'][0]} {a['box'][1]} {a['box'][2]} {a['box'][3]}" for a in image["annotations"]))

    model = YOLO("yolo11n.pt")
    results = model.train(data="coco8.yaml", epochs=5, imgsz=640)
