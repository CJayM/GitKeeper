#pragma once

#include "git_file.h"

#include <QObject>

class Git : public QObject {
  Q_OBJECT
public:
  explicit Git(QString git_path, QObject *parent = nullptr);

  CommandResult status(const QString &workDir);

  signals:

  private:
  QString git_path_;
};
