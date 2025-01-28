import ultralytics
from ultralytics import YOLO

import yaml
import json
import random
import sys

from custom_yolo import CustomTrainer
from pathlib import Path

def run_checks():
    # TODO maybe some other things
    ultralytics.checks()

class NullWriter:
    def write(self, data):
        pass

def train(project_dir, train_args):
    # for TQDM in headless mode
    if sys.stdout is None:
        sys.stdout = NullWriter()

    if sys.stderr is None:
        sys.stderr = NullWriter()

    print("Starting model training......")
    print(f"Arguments: {train_args}")

    training_dir = Path(project_dir) / "train"
    training_dir.mkdir(parents=True, exist_ok=True)
    print(f"Training directory: {training_dir}")

    dataset_config_path = training_dir / "dataset.yaml"

    image_paths = []
    with open(Path(project_dir) / "oceaneye_project_settings.yaml", "r") as file:
        data = yaml.safe_load(file)

        for i in range(data["media"]["size"]):
            image_paths.append(
                data["media"][i+1]["path"]
            )

    print(f"Found {len(image_paths)} images....")
    split_files(training_dir, image_paths)

    yolo_data = {
        "path": "",
        "train": "train.txt",
        "test": "test.txt",
        "val": "val.txt",
        "names": {}
    }

    classes = train_args.pop("classes", {})

    print(f"Classes: {classes}")

    for i, name in classes.items():
        yolo_data["names"][i] = name

    with open(dataset_config_path, 'w') as outfile:
        yaml.dump(yolo_data, outfile, default_flow_style=False)

    model_type = train_args.pop("model", "yolov8m.pt")

    model = YOLO(str(training_dir / "weights" / model_type))

    model.train(
        data=str(dataset_config_path),
        project=str(training_dir),
        trainer=CustomTrainer,
        **train_args
    )

    return model

def split_files(training_dir, image_paths, VALIDATE_SPLIT = 0.15, TEST_SPLIT = 0):
    # Create test/train/validation split
    print("Creating test/train/validation split......")
    TRAIN_SPLIT = 1 - TEST_SPLIT - VALIDATE_SPLIT

    files = image_paths.copy()
    random.shuffle(files)

    # TODO
    """
    BACKGROUND_PERCENT = 0.1

    background_images = list(os.listdir(f"{ANNOTATIONS_DIR}/background/"))
    requested_background_images = int(BACKGROUND_PERCENT * len(data.values()))
    background_images_to_remove = len(background_images) - requested_background_images

    print(f"removing {background_images_to_remove} background images")
    print(len(filenames))
    while background_images_to_remove > 0:
        for file in filenames:
            if file+'.jpg' in background_images:
                filenames.remove(file)
                break
        else:
            break

    """

    """
    files = []
    for f in images:
        if f + '.jpg' in background_images:
            files.append((f, 'background'))
        else:
            files.append((f, 'images'))
    """

    index = lambda s: int((len(files) + 1) * s)

    train_data = files[:index(TRAIN_SPLIT)]
    test_data = files[index(TRAIN_SPLIT):index(TRAIN_SPLIT + TEST_SPLIT)]
    validate_data = files[index(TRAIN_SPLIT + TEST_SPLIT):]

    with open(training_dir / "train.txt", "w") as file:
        file.write("\n".join(train_data))

    with open(training_dir / "test.txt", "w") as file:
        file.write("\n".join(test_data))

    with open(training_dir / "val.txt", "w") as file:
        file.write("\n".join(validate_data))
