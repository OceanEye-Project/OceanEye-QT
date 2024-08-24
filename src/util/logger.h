/**
 * =================================== Logger Class ===================================
 * Custom message log handler. To be initialized ONCE in main.cpp, as QT logging is handled 
 * globally. Cleanup should be called at the end of main.cpp to ensure resources are cleaned up.
 * 
 * This header implementation provides overrides for the following:
 * 
 * - qInfo()
 * - qDebug()
 * - qWarning()
 * - qFatal()
 * - qCritical()
 * 
 * Refer to https://doc.qt.io/qt-6/qtlogging.html#QtMsgType-enum for custom logging with Qt.
 * 
 * Note that when using with standard strings, the QString::fromStdString() function must be used
 * to convert std::string objects into QString objects. To use this functionality, QString should 
 * included.
 * 
 * Using logging in project files within the global scope requires inclusion of only qDebug
 * 
 * ====================================================================================
 * 
 */
#ifndef LOGGER_H  // Logger_H
#define LOGGER_H

// Include necessary Qt headers
#include <QMessageLogger>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <cstdlib>

// Namespace to avoid naming conflicts with other logging utilities
namespace Logger {

    // Logger class declaration
    class Logger {
    public:
        // Public static methods for initializing and cleaning up the logger
        static void init(const QString& filename = "yolov8_log.txt");
        static void cleanup();

    private:
        // Private static method for handling log messages
        static void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

        // Private static members for file handling and state management
        static QFile logFile;
        static QTextStream ls;
        static bool isInitialized;
    };

    } // namespace Logger

#endif // LOGGER_H