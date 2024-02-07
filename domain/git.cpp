#include "git.h"

#include <QDebug>
#include <QProcess>

Git::Git(QString git_path, QObject *parent)
    : QObject{parent}, git_path_(git_path) {}

CommandResult Git::status(const QString &workDir)
{
    CommandResult result;
    result.arguments << "status"
                     << "--porcelain=2";
    auto proccess = new QProcess(this);
    proccess->setWorkingDirectory(workDir);
    proccess->start(git_path_, result.arguments);

    if (!proccess->waitForStarted())
        return result;

    while (proccess->waitForReadyRead()) {
        QByteArray res = proccess->readAll();
        //	TODO: выводить промежуточные данные
        result.result.append(QString(res));
    }
    if (!proccess->waitForFinished())
        return result;

    QByteArray res = proccess->readAll();
    result.result.append(QString(res));
    return result;
}
