#include "git_repository.h"
#include "git.h"
#include "git_file.h"

#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QtConcurrent/QtConcurrent>

const static char *GIT_PATH = "C:\\Program Files\\Git\\cmd\\git.exe";
//  git_ = new Git("C:\\Program Files\\SmartGit\\git\\bin\\git.exe", this);

GitRepository::GitRepository(QObject *parent) : QObject(parent)
{
    git_ = new Git(GIT_PATH, this);

    watcher_ = new QFutureWatcher<QStringList>(this);
    connect(watcher_,
            &QFutureWatcher<QStringList>::progressValueChanged,
            this,
            &GitRepository::onFutureProgress);
    connect(watcher_, &QFutureWatcher<QStringList>::canceled, this, []() {
        qDebug() << "Canceled";
    });
    connect(watcher_, &QFutureWatcher<QStringList>::paused, this, []() { qDebug() << "Paused"; });
    connect(watcher_, &QFutureWatcher<QStringList>::destroyed, this, []() {
        qDebug() << "Destroyed";
    });
    connect(watcher_, &QFutureWatcher<QStringList>::finished, this, &GitRepository::onFinished);

    connect(watcher_,
            &QFutureWatcher<QStringList>::resultReadyAt,
            this,
            &GitRepository::onResultReadyAt);

    connect(watcher_, &QFutureWatcher<QStringList>::resumed, this, []() { qDebug() << "Resumed"; });
    connect(watcher_, &QFutureWatcher<QStringList>::started, this, &GitRepository::onStarted);
}

const QDir &GitRepository::getWorkingDir() const { return workingDir_; }

void GitRepository::setWorkingDir(const QDir &dir) { workingDir_ = dir; }

void GitRepository::status() {
  if (watcher_)
    watcher_->cancel();
  if (future_.isRunning())
    future_.cancel();

  future_ =
      QtConcurrent::run(git_, &Git::status, getWorkingDir().absolutePath());
  watcher_->setFuture(future_);

  //  emit sgnSended(QString("%1 %2").arg(program).arg(arguments.join(" ")));
}

void GitRepository::commit(QString message) {
    QString program = GIT_PATH;
    QStringList arguments;
    arguments << "commit" << QString("--message=%1").arg(message);

    if (gitProcess != nullptr)
        gitProcess->deleteLater();

    gitProcess = new QProcess(this);
    gitProcess->setWorkingDirectory(getWorkingDir().absolutePath());

    connect(gitProcess,
            static_cast<void (QProcess::*)(int exitCode, QProcess::ExitStatus exitStatus)>(
                &QProcess::finished),
            this,
            &GitRepository::onFinish);
    connect(gitProcess, &QProcess::readyRead, this, &GitRepository::onCommitRead);
    connect(gitProcess,
            static_cast<void (QProcess::*)(QProcess::ProcessError)>(&QProcess::error),
            this,
            &GitRepository::onError);

    emit sgnSended(QString("%1 %2").arg(program).arg(arguments.join(" ")));
    gitProcess->start(program, arguments);
}

void GitRepository::onStatusRead() {
  qDebug() << "ReadyRead";
  auto normal = gitProcess->readAllStandardOutput();

  emit sgnReceived(normal, false);
  proccessGitStatus(normal);
}

void GitRepository::onCommitRead() {
  qDebug() << "CommitRead";
  auto normal = gitProcess->readAllStandardOutput();
  if (normal.isNull() == false) {
    emit sgnReceived(normal, false);
    //	proccessGitStatus(normal);
  }
}

void GitRepository::onFinish(int exitCode, QProcess::ExitStatus exitStatus) {
  auto normal = gitProcess->readAllStandardOutput();
  auto error = gitProcess->readAllStandardError();
  if (error.isNull() == false) {
    emit sgnReceived(error, true);
  }
  if (normal.isNull() == false) {
    Q_ASSERT(normal.isEmpty());
    emit sgnReceived(normal, false);
  }
  gitProcess->deleteLater();
  gitProcess = nullptr;
}

void GitRepository::onError(QProcess::ProcessError error) {
  auto message = gitProcess->readAllStandardOutput();
  emit sgnReceived(message, true);
  qDebug() << error;
}

void GitRepository::onStarted() { qDebug() << "onStarted"; }

void GitRepository::onFutureProgress(int progressValue) {
  qDebug() << "onFutureProgress" << progressValue;
}
void GitRepository::onResultReadyAt(int resultIndex) {
  qDebug() << "onResultReadyAt" << resultIndex;
  auto res = future_.resultAt(resultIndex);
  auto files = proccessGitStatus(res.join("\n"));
  emit sgnResultReceived(files);
}
void GitRepository::onFinished() {
  qDebug() << "onFinished";
  emit sgnFinished();
}

QVector<GitFile> GitRepository::proccessGitStatus(QString data) {
  auto lines = data.split("\n");

  QVector<GitFile> files;

  for (const auto &line : lines) {
    if (line.isEmpty())
      continue;
    GitFile file;
    if (line[0] == '1') {
      file.indexState = charToState(line[2]);
      file.workState = charToState(line[3]);
      file.submoduleState = line.mid(5, 4);
      file.fileModeHead = line.mid(10, 6);
      file.fileModeIndex = line.mid(17, 6);
      file.fileModeWorkTree = line.mid(24, 6);
      file.nameHead = line.mid(31, 40);
      file.nameIndex = line.mid(72, 40);
      file.name = line.mid(113);
    }
    if (line[0] == '?') {
      file.name = line.mid(2);
      file.indexState = FileState::UNTRACKED;
      file.workState = charToState(line[0]);
    }

    auto res = file.name.split("/");
    file.name = res.last();
    if (res.size() > 0)
      file.path = res.mid(0, res.size() - 1).join(QDir::separator());
    files.append(file);
  }

  return files;
}
