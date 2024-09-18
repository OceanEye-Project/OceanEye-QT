#include "videoslicer.h"
#include <QDebug>
#include <QString>

VideoSlicer::VideoSlicer(std::shared_ptr<Project>& project)
    : QObject{}
    , currentProject(project)
    , dialog("Slicing Video(s)...")
{
    connect(&watcher, &QFutureWatcher<std::vector<QString>>::finished, this, [this]{
        qInfo() << "all threads done, results: " << future.resultCount();

        for (int i=0; i<future.resultCount(); i++) {
            std::vector<QString> newImages = future.resultAt(i);
            currentProject->media.insert(currentProject->media.end(), newImages.begin(), newImages.end());
        }
        currentProject->saveMedia();
        emit doneSlicing();
        dialog.hide();
    });

    connect(this, &VideoSlicer::updateProgress, this, [this](int progress){
        dialog.updateProgress(dialog.currentProgress + progress);
    });

}

std::vector<QString> VideoSlicer::sliceVideo(const QString& video, const QString& projectPath) {
    qInfo() << "Commence Slicing " << video;

    cv::Mat frame;
    std::string framePath;

    std::vector<QString> savedFrames {};

    cv::VideoCapture cap(video.toStdString());

    if (!cap.isOpened()) {
        qWarning() << "Error opening video file " << video;
    }

    double fps = cap.get(cv::CAP_PROP_FPS);
    int skipSeconds = currentProject->settings.value("Slice Interval").toInt();
    int frameInterval = static_cast<int>(fps * skipSeconds);

    bool result;
    int currentFrame = 0;

    std::filesystem::path videoPath(video.toStdString());
    std::filesystem::path stillsPath(projectPath.toStdString());


    while (cap.read(frame)) {
        emit updateProgress(frameInterval);
        framePath = (stillsPath / (videoPath.stem().string() + "_" + std::to_string(currentFrame) + ".jpeg")).string();

        // Check if the file already exists
        // If it doesn't, continue writing
        if (!std::filesystem::exists(framePath))
        {
            result = cv::imwrite(framePath, frame);
            if (!result) {
                qWarning() << "Failed to save frame: " << QString::fromStdString(framePath);
            } else {
                bool annotationsExist = currentProject->runDetection(QString::fromStdString(framePath));
                if (currentProject->settings.contains("Automatically Filter Dead Video")) {
                    if (currentProject->settings.value("Automatically Filter Dead Video") == true) {
                        if (annotationsExist) {
                            savedFrames.push_back(QString::fromStdString(framePath));
                            qInfo() << "Filtered Frame saved successfully: " << currentFrame << " / " << cap.get(cv::CAP_PROP_FRAME_COUNT) << " " << QString::fromStdString(framePath);
                        }
                    } 
                    else {
                        savedFrames.push_back(QString::fromStdString(framePath));
                        qInfo() << "Unfiltered Frame saved successfully: " << currentFrame << " / " << cap.get(cv::CAP_PROP_FRAME_COUNT) << " " << QString::fromStdString(framePath);
                    }
                }
            }
        }
        else {
            if (currentProject->settings.contains("Automatically Filter Dead Video")) {
                if (currentProject->settings.value("Automatically Filter Dead Video") == true) {
                    bool annotationsExist = currentProject->runDetection(QString::fromStdString(framePath));
                    if (annotationsExist) {
                        savedFrames.push_back(QString::fromStdString(framePath));
                       qInfo() << "Filtered Frame saved successfully: " << currentFrame << " / " << cap.get(cv::CAP_PROP_FRAME_COUNT) << " " << QString::fromStdString(framePath);
                    }
                } 
                else {
                    savedFrames.push_back(QString::fromStdString(framePath));
                    qInfo() << "Unfiltered Frame saved successfully: " << currentFrame << " / " << cap.get(cv::CAP_PROP_FRAME_COUNT) << " " << QString::fromStdString(framePath);
                }
            }
        }

        currentFrame += frameInterval;
        cap.set(cv::CAP_PROP_POS_FRAMES, currentFrame);
    }
    qInfo() << "Done slicing video " << video;

    return savedFrames;
}

QFuture<std::vector<QString>> VideoSlicer::slice(QStringList videosToSlice) {
    if (videosToSlice.size() > 0) {
        dialog.show();

        int totalFrames = 0;

        for (auto& video : videosToSlice) {
            cv::VideoCapture cap(video.toStdString());
            totalFrames += cap.get(cv::CAP_PROP_FRAME_COUNT);
            cap.release();
        }

        dialog.setRange(0, totalFrames);
        dialog.updateProgress(0);

        std::function<std::vector<QString>(const QString&)> sliceVideoWithProject = [this](const QString& video) {
            return sliceVideo(video, currentProject->projectPath);
        };

        future = QtConcurrent::mapped(videosToSlice, sliceVideoWithProject);
        watcher.setFuture(future);
    }
    return future;
}

