# OceanEye

### SOLIDS Lab, UVic
OceanEye is a C++ desktop application built with Qt for computer aided image annotation.

## Requirements

Requires `Qt6` and `OpenCv`. On MacOs, both can be installed with `brew install opencv qt6`.

## Building & Running

### MacOs
1. Create output directory and configure CMake
```bash
mkdir build
cmake -H. -B build
```
2. build project
```bash
cmake --build build --target OceanEye
```
3. run `OceanEye.app`
```bash
open build/OceanEye.app
```

### Windows
Instructions coming soon!

## Directory Structure

```
OceanEye
├── models         - Pretrained ONNX YOLOv8 models
├── src            - Application code
│   ├── core       - Main application windows
│   ├── gui        - Custom widgets and layouts, etc.
│   ├── resources  - Images, styles, etc. (non-code application files)
│   └── util       - Non-Qt specific code
└── tests          - QTest files
```
