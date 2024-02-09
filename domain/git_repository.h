#pragma once

#include "git_file.h"

#include "app_settings.h"
#include <QDir>
#include <QFuture>
#include <QFutureWatcher>
#include <QProcess>
#include <QVector>

class Git;

enum class GIT_COMMAND { NO, COMMIT, STATUS, LAST_MESSAGE };

class GitRepository : public QObject
{
    Q_OBJECT
public:
    GitRepository(AppSettings &settings, QObject *parent);
    const QDir &getWorkingDir() const;
    void setWorkingDir(const QDir &dir);

    void status();
    void commit(QString message, bool isAmend);
    void requestLastCommitMessage();

signals:

    void sgnSended(QString data);
    void sgnReceived(QString data, bool isError);

    void sgnResultReceived(QVector<GitFile> result);
    void sgnFinished();
    void sgnLastMessageReady(QString msg);

private slots:
  void onFutureProgress(int progressValue);
  void onFinished();
  void onResultReadyAt(int resultIndex);
  void onStarted();

  private:
  QDir workingDir_;
  AppSettings &settings_;
  Git *git_ = nullptr;
  QFuture<CommandResult> future_;
  QFutureWatcher<CommandResult> *watcher_;

  GIT_COMMAND currentCommand_ = GIT_COMMAND::NO;

  QVector<GitFile> proccessGitStatus(QString data);
  void proccessLastMessage(QString text);
};
