#ifndef VIDEOSLICER_H
#define VIDEOSLICER_H

#include <QObject>
#include <QtConcurrent/QtConcurrent>
#include "waitingdialog.h"
#include "project.h"

class VideoSlicer : public QObject
{
    Q_OBJECT
    WaitingDialog dialog;

    QFutureWatcher<std::vector<QString>> watcher;
    QFuture<std::vector<QString>> future;
    std::shared_ptr<Project>& currentProject;

    std::vector<QString> sliceVideo(const QString& video, const QString& projectPath);

public:
    explicit VideoSlicer(std::shared_ptr<Project>& project);
    void slice(QStringList videos);

signals:
    void updateProgress(int value);
    void doneSlicing();
};

#endif // VIDEOSLICER_H
