#include "yolov8.h"
#include <qDebug>
#include <QString>


std::string scanUntil(std::ifstream &inputFile, const char pattern, int& i, int length, const std::function <bool (char)>& f) {
    std::string value {};
    char byte;
    inputFile.read(reinterpret_cast<char*>(&byte), 1);
    i++;
    while (byte != pattern && i < length) {
        if (f(byte)) {
            value += byte;
        }
        inputFile.read(reinterpret_cast<char*>(&byte), 1);
        ++i;
    }
    return value;
}

std::string scanUntil(std::ifstream &inputFile, const char pattern, int& i, int length) {
    return scanUntil(inputFile, pattern, i, length, [](char c) { return true; });
}

std::string scanUntil(std::string s, int& i, const std::function <bool (char)>& f) {
    std::string value {};
    while (f(s[i])) {
        value += s[i];
        i++;
    }
    return value;
}

std::string scanUntil(std::string s, int& i, char c) {
    return scanUntil(s, i, [&](char n) { return n != c; });
}

std::vector<QString> YOLOv8::loadClasses() {
    return loadClasses(modelPath);
}

std::vector<QString> YOLOv8::loadClasses(std::string path) {
    qInfo() << "Beginning to load classes... ";

    bool match = false;

    // https://protobuf.dev/programming-guides/encoding/
    // https://onnx.ai/onnx/api/classes.html#modelproto
    // "Names", plus the start of the next item
    std::vector<char> magicNumber = {0x6e, 0x61, 0x6d, 0x65, 0x73, 0x12};
    
    std::vector<QString> classes {};
    std::map<int, std::string> classMap {};

    qInfo() << "Attempting to open model to read classes: " << QString::fromStdString(path);

    std::ifstream inputFile(path, std::ios_base::in | std::ios::binary);

    if (!inputFile.is_open()) {
        qCritical() << "Error opening the file" << QString::fromStdString(path);
        return {};
    }

    qInfo() << "Successfully Opened Model";

    std::istreambuf_iterator<char> fileBegin{inputFile}, fileEnd{};

    auto n = std::search(fileBegin, fileEnd, magicNumber.begin(), magicNumber.end());

    bool found = n != fileEnd;

    if (!found) {
        qInfo() << "Could not find classes";
        return {};
    }

    std::string classesString(n, fileEnd);

    auto jsonStart = classesString.find('{');
    auto jsonEnd = classesString.find('}');
    
    classesString = classesString.substr(jsonStart, jsonEnd - jsonStart);

    for (int i = 0; i < classesString.size(); ++i) {
        scanUntil(classesString, i, [](char c) { return !isdigit(c); });
        std::string classId = scanUntil(classesString, i, ':');
        scanUntil(classesString, i, '\'');
        std::string className = scanUntil(classesString, ++i, '\'');
        classMap[std::stoi(classId)] = className;
    }
    
    inputFile.close();

    classes.resize(classMap.size());
    for (auto const& [key, value] : classMap) {
        classes[key] = QString::fromStdString(value);
        qInfo() << "Class: " << key << " " << value;
    }

    return classes;


}

YOLOv8::YOLOv8(const std::string &onnxModelPath, const cv::Size &modelInputShape, const bool &runWithCuda)
{
    modelPath = onnxModelPath;
    modelShape = modelInputShape;
    cudaEnabled = runWithCuda;
}

std::vector<Annotation> YOLOv8::runInference(const cv::Mat &input, const std::vector<QString>& model_classes) {
    if (!loaded) {
        qWarning() << "Error: Model is not loaded";
        return {};
    }

    cv::Mat modelInput = input;
    if (letterBoxForSquare && modelShape.width == modelShape.height)
        modelInput = formatToSquare(modelInput);

    cv::Mat blob;
    cv::dnn::blobFromImage(modelInput, blob, 1.0/255.0, modelShape, cv::Scalar(), true, false);
    net.setInput(blob);

    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());

    int rows = outputs[0].size[1];
    int dimensions = outputs[0].size[2];

    bool yolov8 = false;
    // yolov5 has an output of shape (batchSize, 25200, 85) (Num classes + box[x,y,w,h] + confidence[c])
    // yolov8 has an output of shape (batchSize, 84,  8400) (Num classes + box[x,y,w,h])
    if (dimensions > rows) // Check if the shape[2] is more than shape[1] (yolov8)
    {
        yolov8 = true;
        rows = outputs[0].size[2];
        dimensions = outputs[0].size[1];

        outputs[0] = outputs[0].reshape(1, dimensions);
        cv::transpose(outputs[0], outputs[0]);
    }
    float *data = (float *)outputs[0].data;

    float x_factor = modelInput.cols / modelShape.width;
    float y_factor = modelInput.rows / modelShape.height;

    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    for (int i = 0; i < rows; ++i)
    {
        if (yolov8)
        {
            float *classes_scores = data+4;

            cv::Mat scores(1, model_classes.size(), CV_32FC1, classes_scores);
            cv::Point class_id;
            double maxClassScore;

            minMaxLoc(scores, 0, &maxClassScore, 0, &class_id);
            if (maxClassScore > modelScoreThreshold)
            {
                confidences.push_back(maxClassScore);
                class_ids.push_back(class_id.x);

                float x = data[0];
                float y = data[1];
                float w = data[2];
                float h = data[3];

                int left = int((x - 0.5 * w) * x_factor);
                int top = int((y - 0.5 * h) * y_factor);

                int width = int(w * x_factor);
                int height = int(h * y_factor);

                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }
        else // yolov5
        {
            float confidence = data[4];

            if (confidence >= modelConfidenceThreshold)
            {
                float *classes_scores = data+5;

                cv::Mat scores(1, model_classes.size(), CV_32FC1, classes_scores);
                cv::Point class_id;
                double max_class_score;

                minMaxLoc(scores, 0, &max_class_score, 0, &class_id);

                if (max_class_score > modelScoreThreshold)
                {
                    confidences.push_back(confidence);
                    class_ids.push_back(class_id.x);

                    float x = data[0];
                    float y = data[1];
                    float w = data[2];
                    float h = data[3];

                    int left = int((x - 0.5 * w) * x_factor);
                    int top = int((y - 0.5 * h) * y_factor);

                    int width = int(w * x_factor);
                    int height = int(h * y_factor);

                    boxes.push_back(cv::Rect(left, top, width, height));
                }
            }
        }

        data += dimensions;
    }

    std::vector<int> nms_result;
    cv::dnn::NMSBoxes(boxes, confidences, modelScoreThreshold, modelNMSThreshold, nms_result);
    
    qInfo() << "Beginning detections...";

    std::vector<Annotation> annotations {};
    for (unsigned long i = 0; i < nms_result.size(); ++i)
    {
        int idx = nms_result[i];

        Annotation result {};
        result.classId = class_ids[idx];
        result.className = model_classes[class_ids[idx]];

        result.confidence = confidences[idx];

        result.box.setRect(
            boxes[idx].x,
            boxes[idx].y,
            boxes[idx].width,
            boxes[idx].height
        );
        // Out results
        qInfo() << "Detected: " << result.className << " ID: " << result.classId << " Conf: " << result.confidence;
        annotations.push_back(result);
    }
    
    qInfo() << "Completed annotation for current media";
    return annotations;
}


void YOLOv8::loadOnnxNetwork() {
    qInfo() << "Loading ONNX model from: " << QString::fromStdString(modelPath);
    try {
        net = cv::dnn::readNetFromONNX(modelPath);
    } catch (const cv::Exception &e) {
        qCritical() << "Error loading the ONNX model: " << QString::fromStdString(e.what()); 
        qErrnoWarning("Error loading the ONNX model. Check error logs for more information");
        loaded = false;
        return;
    }

    if (cudaEnabled) {
        qInfo() << "Running on CUDA";

        net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
    } else {
        qInfo() << "Running on CPU";
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    }

    loaded = true;
    qInfo() << "Model loaded successfully.";
}

cv::Mat YOLOv8::formatToSquare(const cv::Mat &source) {
    int col = source.cols;
    int row = source.rows;
    int _max = MAX(col, row);
    cv::Mat result = cv::Mat::zeros(_max, _max, CV_8UC3);
    source.copyTo(result(cv::Rect(0, 0, col, row)));
    return result;
}

void YOLOv8::drawDetections(const std::vector<Annotation>& annotations, cv::Mat& frame) {
    for (auto& annotation: annotations) {
        cv::Rect box {};

        box.x = annotation.box.x();
        box.y = annotation.box.y();
        box.width = annotation.box.width();
        box.height = annotation.box.height();

        cv::Scalar color {255, 0, 0, 255};

        // Annotation box
        cv::rectangle(frame, box, color, 2);

        // Annotation box text
        std::string classString = annotation.className.toStdString() + ' ' + std::to_string(annotation.confidence).substr(0, 4);
        cv::Size textSize = cv::getTextSize(classString, cv::FONT_HERSHEY_DUPLEX, 1, 2, 0);
        cv::Rect textBox(box.x, box.y - 40, textSize.width + 10, textSize.height + 20);

        cv::rectangle(frame, textBox, color, cv::FILLED);
        cv::putText(frame, classString, cv::Point(box.x + 5, box.y - 10), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 0), 2, 0);
    }
}
