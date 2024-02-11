#pragma once

#include "git_file.h"

#include <QObject>

class Git : public QObject
{
    Q_OBJECT
public:
    explicit Git(QObject *parent = nullptr);

    CommandResult execute(const QString &workDir, const QString &gitPath, const QStringList &params);
    QStringList makeStatusCommand() const;
    QStringList makeCommitCommand(QString message, bool isAmend) const;
    QStringList makeLastCommitMessageCommand() const;
    QStringList makeShowCommand(QString filepath) const;
    QStringList makeShowDiffCommand(QString filepath) const;

signals:

private:
};
