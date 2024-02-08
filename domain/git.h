#pragma once

#include "git_file.h"

#include <QObject>

class Git : public QObject
{
    Q_OBJECT
public:
    explicit Git(QObject *parent = nullptr);

    CommandResult status(const QString &workDir, const QString &gitPath);

signals:

private:
};
