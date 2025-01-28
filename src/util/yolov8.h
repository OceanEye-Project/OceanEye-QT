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
#include <QShortcut>


struct Annotation {
    int classId;
    QString className;
    float confidence;
    QRect box;
};

class YOLOv8 {
public:
    YOLOv8(
        const std::string &onnxModelPath = "",
        const cv::Size &modelInputShape = {640, 640},            
        const bool &runWithCuda = true);
        
    std::vector<Annotation> runInference(const cv::Mat &input, const std::vector<QString>& model_classes);
    void drawDetections(const std::vector<Annotation>& annotations, cv::Mat &input);
    void loadOnnxNetwork();
    bool loaded {false};
    std::string modelPath{};
    float modelConfidenceThreshold {0.25};
    float modelScoreThreshold      {0.45};
    float modelNMSThreshold        {0.50};
    std::vector<QString> loadClasses();
    static std::vector<QString> loadClasses(std::string path);

private:
    cv::Mat formatToSquare(const cv::Mat &source);

    std::string classesPath{};
    bool cudaEnabled {false};

    cv::Size2f modelShape{};

    bool letterBoxForSquare = true;

    cv::dnn::Net net;

};

#endif //YOLOV8_H
