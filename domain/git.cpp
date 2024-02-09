#include "git.h"

#include <QDebug>
#include <QProcess>

Git::Git(QObject *parent) : QObject{parent} {}

CommandResult Git::execute(const QString &workDir, const QString &gitPath, const QStringList &params)
{
    CommandResult result;
    result.arguments = params;

    auto proccess = new QProcess(this);
    proccess->setWorkingDirectory(workDir);
    proccess->start(gitPath, params);

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

QStringList Git::makeStatusCommand() const
{
    QStringList result;
    result << "status"
           << "--porcelain=2";
    return result;
}

QStringList Git::makeCommitCommand(QString message) const
{
    QStringList result;
    result << "commit"
           << "--message=" + message;
    return result;
}
