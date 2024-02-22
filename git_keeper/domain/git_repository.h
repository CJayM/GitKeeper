#pragma once

#include "git_file.h"

#include <QDir>
#include <QFuture>
#include <QFutureWatcher>
#include <QProcess>
#include <QVector>

class Git;

enum class GIT_COMMAND { NO, COMMIT, LAST_MESSAGE };

class GitRepository : public QObject
{
    Q_OBJECT
public:
    GitRepository(const QString &gitPath, QObject *parent);
    const QDir &getWorkingDir() const;
    void setWorkingDir(const QDir &dir);

    void status();
    void commit(QString message, bool isAmend);
    void requestLastCommitMessage();
    void queryFile(QString filepath);

    void setGitPath(const QString &path);

signals:
    void sgnSended(QString data);
    void sgnReceived(QString data, bool isError);

    void sgnResultReceived(QVector<GitFile> result);
    void sgnFinished();
    void sgnLastMessageReady(QString msg);
    void sgnCurrentFuleContentReaded(QString filepath, QString before, QString after);
    void sgnDiffsReaded(QStringList data);

    void sgnBlockStatusAndDiff(
        bool isBlocked); // сигнал блокирования в связи с процессом получения статуса или diff

private slots:
  void onFutureProgress(int progressValue);
  void onFinished();
  void onResultReadyAt(int resultIndex);
  void onStarted();

  private:
  QDir workingDir_;
  QString gitPath_;
  Git *git_ = nullptr;
  QFuture<CommandResult> future_;
  QFutureWatcher<CommandResult> *watcher_;

  GIT_COMMAND currentCommand_ = GIT_COMMAND::NO;

  QVector<GitFile> proccessGitStatus(QString data);
  void proccessLastMessage(QString text);
  QFuture<std::pair<QString, QString>> readCurrentFile(QString filepath);
  QFuture<std::pair<QString, QString>> readStagedOrCommitedFile(QString filepath);
  QFuture<std::pair<QString, QString>> readDiffFile(QString filepath);
};
