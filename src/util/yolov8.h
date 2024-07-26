#ifndef YOLOV8_H
#define YOLOV8_H

#include <fstream>
#include <vector>
#include <array>
#include <string>
#include <random>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <QString>
#include <QRect>

struct Annotation {
    int classId;
    QString className;
    float confidence;
    QRect box;
};

inline static std::array<std::string, 5> model_classes = {"Kelp", "Sea Urchin", "Sea Star", "Fish", "Sea Cucumber"};

class YOLOv8 {
public:
    YOLOv8(
        const std::string &onnxModelPath = "",
        const cv::Size &modelInputShape = {640, 640},
        const std::string &classesTxtFile = "",
        const bool &runWithCuda = true);
    std::vector<Annotation> runInference(const cv::Mat &input);
    void drawDetections(const std::vector<Annotation>& annotations, cv::Mat &input);
    void loadOnnxNetwork();
    bool loaded {false};
    std::map<int, std::string> classMap;
    std::string modelPath{};
    float modelConfidenceThreshold {0.25};
    float modelScoreThreshold      {0.45};
    float modelNMSThreshold        {0.50};
    void loadClasses();
private:
    //        void loadClassesFromFile();
    cv::Mat formatToSquare(const cv::Mat &source);

    std::string classesPath{};
    bool cudaEnabled {false};
    //        std::vector<std::string> classes{"person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light", "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple", "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch", "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone", "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear", "hair drier", "toothbrush"};

    cv::Size2f modelShape{};

    bool letterBoxForSquare = true;

    cv::dnn::Net net;

};

#endif //YOLOV8_H
