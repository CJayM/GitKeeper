#include "git.h"

#include <QDebug>
#include <QProcess>

Git::Git(QObject *parent) : QObject{parent} {}

CommandResult Git::status(const QString &workDir, const QString &gitPath)
{
    CommandResult result;
    result.arguments << "status"
                     << "--porcelain=2";
    auto proccess = new QProcess(this);
    proccess->setWorkingDirectory(workDir);
    proccess->start(gitPath, result.arguments);

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
