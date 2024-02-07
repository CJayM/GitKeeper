#pragma once

#include "git_file.h"

#include "app_settings.h"
#include <QDir>
#include <QFuture>
#include <QFutureWatcher>
#include <QProcess>
#include <QVector>

class Git;

class GitRepository : public QObject {
  Q_OBJECT
public:
    GitRepository(AppSettings &settings, QObject *parent);
    const QDir &getWorkingDir() const;
    void setWorkingDir(const QDir &dir);

    void status();
    void commit(QString message);

signals:

  void sgnSended(QString data);
  void sgnReceived(QString data, bool isError);

  void sgnResultReceived(QVector<GitFile> result);
  void sgnFinished();

private slots:
  void onStatusRead();
  void onCommitRead();
  void onFinish(int exitCode, QProcess::ExitStatus exitStatus);
  void onError(QProcess::ProcessError error);

  void onFutureProgress(int progressValue);
  void onFinished();
  void onResultReadyAt(int resultIndex);
  void onStarted();

private:
  QDir workingDir_;
  QProcess *gitProcess = nullptr;
  AppSettings &settings_;
  Git *git_ = nullptr;
  QFuture<QStringList> future_;
  QFutureWatcher<QStringList> *watcher_;

  QVector<GitFile> proccessGitStatus(QString data);
};
