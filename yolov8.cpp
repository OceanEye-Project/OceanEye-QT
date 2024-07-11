#include "yolov8.h"

YOLOv8::YOLOv8(const std::string &onnxModelPath, const cv::Size &modelInputShape, const std::string &classesTxtFile, const bool &runWithCuda)
{
    modelPath = onnxModelPath;
    modelShape = modelInputShape;
    classesPath = classesTxtFile;
    cudaEnabled = runWithCuda;
    // loadClassesFromFile(); The classes are hard-coded for this example
}

std::vector<Annotation> YOLOv8::runInference(const cv::Mat &input) {
    if (!loaded) {
        std::cout << "Error: Model is not loaded" << std::endl;
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

            cv::Mat scores(1, classes.size(), CV_32FC1, classes_scores);
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

                cv::Mat scores(1, classes.size(), CV_32FC1, classes_scores);
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

    std::vector<Annotation> annotations {};
    for (unsigned long i = 0; i < nms_result.size(); ++i)
    {
        int idx = nms_result[i];

        Annotation result {};
        result.classId = class_ids[idx];
        result.className = QString::fromStdString(classes[class_ids[idx]]);

        result.confidence = confidences[idx];

        result.box.x = boxes[idx].x;
        result.box.y = boxes[idx].y;
        result.box.w = boxes[idx].width;
        result.box.h = boxes[idx].height;

        annotations.push_back(result);
    }

    return annotations;
}

//void YOLOv8::loadClassesFromFile()
//{
//    std::ifstream inputFile(classesPath);
//    if (inputFile.is_open())
//    {
//        std::string classLine;
//        while (std::getline(inputFile, classLine))
//            classes.push_back(classLine);
//        inputFile.close();
//    }
//}

void YOLOv8::loadOnnxNetwork() {
    std::cout << "Loading ONNX model from: " << modelPath << std::endl;
    try {
        net = cv::dnn::readNetFromONNX(modelPath);
    } catch (const cv::Exception &e) {
        std::cerr << "Error loading the ONNX model: " << e.what() << std::endl;
        loaded = false;
        return;
    }

    if (cudaEnabled) {
        std::cout << "\nRunning on CUDA" << std::endl;
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
    } else {
        std::cout << "\nRunning on CPU" << std::endl;
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    }

    loaded = true;
    std::cout << "Model loaded successfully." << std::endl;
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

        box.x = annotation.box.x;
        box.y = annotation.box.y;
        box.width = annotation.box.w;
        box.height = annotation.box.h;

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
