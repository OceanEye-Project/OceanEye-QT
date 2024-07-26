#include "videoslicer.h"

VideoSlicer::VideoSlicer(std::shared_ptr<Project>& project)
    : QObject{}
    , currentProject(project)
    , dialog("Slicing Video(s)...")
{
    connect(&watcher, &QFutureWatcher<std::vector<QString>>::finished, this, [this]{
        std::cout << "all threads done, resuts: " << future.resultCount() << std::endl;
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
    std::cout << "Slicing " << video.toStdString() << std::endl;

    cv::Mat frame;
    std::string framePath;

    std::vector<QString> savedFrames {};

    cv::VideoCapture cap(video.toStdString());

    if (!cap.isOpened()) {
        std::cerr << "error opening video file";
    }

    double fps = cap.get(cv::CAP_PROP_FPS);
    int skipSeconds = 5;
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
                std::cerr << "Failed to save frame: " << framePath << std::endl;
            } else {
                savedFrames.push_back(QString::fromStdString(framePath));
                std::cout << "Frame saved successfully: " << currentFrame << " / " << cap.get(cv::CAP_PROP_FRAME_COUNT) << " " << framePath << std::endl;
            }
        }
        else {
            std::cout << "Frame already exists: " << framePath << std::endl;
            savedFrames.push_back(QString::fromStdString(framePath));
        }

        currentFrame += frameInterval;
        cap.set(cv::CAP_PROP_POS_FRAMES, currentFrame);
    }
    std::cout << "Done slicing video" << std::endl;

    return savedFrames;
}

void VideoSlicer::slice(QStringList videosToSlice) {
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
}

