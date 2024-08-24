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
#include "logger.h"

namespace Logger {

    // Initialize static members
    QFile Logger::logFile;
    QTextStream Logger::ls;
    bool Logger::isInitialized = false;

    // Initialize the logger
    void Logger::init(const QString& filename) {
        // Prevent multiple initializations
        if (isInitialized) return;

        // Delete the existing log file if it exists
        QFile existingFile(filename);
        if (existingFile.exists()) {
            existingFile.remove();
        }

        logFile.setFileName(filename);
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
            ls.setDevice(&logFile);
            // Init custom message handler
            qInstallMessageHandler(customMessageHandler);
            isInitialized = true;

            // Log the initialization
            QDateTime currentTime = QDateTime::currentDateTime();
            QString initMessage = currentTime.toString("yyyy-MM-dd hh:mm:ss.zzz ") + "Logger initialized. New log file created.\n";
            ls << initMessage;
            ls.flush();
        } else {
            // Fallback to default logging if file can't be opened
            qWarning() << "Could not open log file. Using default logging.";
        }
    }

    // Clean up the logger
    void Logger::cleanup() {
        if (isInitialized) {
            ls.flush();  // Ensure all data is written
            logFile.close();
            // Restore the default message handler
            qInstallMessageHandler(nullptr);
            isInitialized = false;
        }
    }

    // Custom message handler to format and write log messages
    void Logger::customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        if (!isInitialized) return;  // Safety check

        QByteArray localMsg = msg.toLocal8Bit();
        const char *file = context.file ? context.file : "";
        const char *function = context.function ? context.function : "";
        QString txtMessage;

        // Format message based on its type
        switch (type) {
        case QtDebugMsg:
            txtMessage = QString("%1").arg(localMsg.constData());
            break;
        case QtInfoMsg:
            txtMessage = QString("Info: %1").arg(localMsg.constData());
            break;
        case QtWarningMsg:
            txtMessage = QString("Warning: %1").arg(localMsg.constData());
            break;
        case QtCriticalMsg:
            txtMessage = QString("Critical: %1").arg(localMsg.constData());
            break;
        case QtFatalMsg:
            txtMessage = QString("Fatal: %1 (%2:%3, %4)").arg(localMsg.constData()).arg(file).arg(context.line).arg(function);
            ls << txtMessage << Qt::endl;
            ls.flush();
            std::abort();  // Terminate the program on fatal errors
        }

        // Add timestamp to the message
        QDateTime currentTime = QDateTime::currentDateTime();
        QString formattedMessage = currentTime.toString("yyyy-MM-dd hh:mm:ss.zzz ") + txtMessage + "\n";
        
        // Write to log file and flush immediately
        ls << formattedMessage;
        ls.flush();
    }

} // namespace Logger