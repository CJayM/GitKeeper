#include "git.h"

#include <QDebug>
#include <QProcess>

Git::Git(QString git_path, QObject *parent)
    : QObject{parent}, git_path_(git_path) {}

QStringList Git::status(const QString &workDir) {
  QStringList arguments;
  arguments << "status"
            << "--porcelain=2";
  auto proccess = new QProcess(this);
  proccess->setWorkingDirectory(workDir);
  proccess->start(git_path_, arguments);

  QStringList result;

  if (!proccess->waitForStarted())
    return result;

  while (proccess->waitForReadyRead()) {
    QByteArray res = proccess->readAll();
    //	TODO: выводить промежуточные данные
    result.append(QString(res));
  }
  if (!proccess->waitForFinished())
    return result;

  QByteArray res = proccess->readAll();
  result.append(QString(res));
  return result;
}
