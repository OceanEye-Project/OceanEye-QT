// ======================== Logger ========================
/**
 * 
 */

#include <QMessageLogger>
#include <QFile>
#include <QTextStream>
#include <QDateTime>


// Set up logger
QFile logFile("log.txt");
QTextStream ls;

// Message handler
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    QString txtMessage;

    switch (type) {
    case QtDebugMsg:
        txtMessage = QString("Debug: %1 (%2:%3, %4)").arg(localMsg.constData()).arg(file).arg(context.line).arg(function);
        break;
    case QtInfoMsg:
        txtMessage = QString("Info: %1 (%2:%3, %4)").arg(localMsg.constData()).arg(file).arg(context.line).arg(function);
        break;
    case QtWarningMsg:
        txtMessage = QString("Warning: %1 (%2:%3, %4)").arg(localMsg.constData()).arg(file).arg(context.line).arg(function);
        break;
    case QtCriticalMsg:
        txtMessage = QString("Critical: %1 (%2:%3, %4)").arg(localMsg.constData()).arg(file).arg(context.line).arg(function);
        break;
    case QtFatalMsg:
        txtMessage = QString("Fatal: %1 (%2:%3, %4)").arg(localMsg.constData()).arg(file).arg(context.line).arg(function);
        abort();
    }

    QDateTime currentTime = QDateTime::currentDateTime();
    QString formattedMessage = currentTime.toString("yyyy-MM-dd hh:mm:ss.zzz ") + txtMessage + "\n";
    ls << formattedMessage;
    ls.flush();
}
// End of logging class. 