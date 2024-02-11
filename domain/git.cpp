#include "git.h"

#include <QDebug>
#include <QProcess>

Git::Git(QObject *parent) : QObject{parent} {}

CommandResult Git::execute(const QString &workDir, const QString &gitPath, const QStringList &params)
{
    CommandResult result;
    result.arguments = params;

    auto proccess = new QProcess();
    proccess->setWorkingDirectory(workDir);
    proccess->start(gitPath, params);

    if (!proccess->waitForStarted()) {
        proccess->deleteLater();
        return result;
    }

    while (proccess->waitForReadyRead()) {
        QByteArray res = proccess->readAll();
        //	TODO: выводить промежуточные данные
        result.result.append(QString(res));
    }

    if (!proccess->waitForFinished()) {
        proccess->deleteLater();
        return result;
    }

    QByteArray res = proccess->readAll();
    result.result.append(QString(res));
    proccess->deleteLater();
    return result;
}

QStringList Git::makeStatusCommand() const
{
    QStringList result;
    result << "status"
           << "--porcelain=2";
    return result;
}

QStringList Git::makeCommitCommand(QString message, bool isAmend) const
{
    QStringList result;
    result << "commit"
           << "--message=" + message;
    if (isAmend)
        result << "--amend";
    return result;
}

QStringList Git::makeLastCommitMessageCommand() const
{
    QStringList result;
    result << "log"
           << "-1"
           << "--oneline";
    return result;
}

QStringList Git::makeShowCommand(QString filepath) const
{
    QStringList result;
    result << "show"
           << "HEAD:" + filepath;
    return result;
}

QStringList Git::makeShowDiffCommand(QString filepath) const
{
    QStringList result;
    result << "diff"
           << "--no-color"
           << "-U0" << filepath;
    return result;
}
