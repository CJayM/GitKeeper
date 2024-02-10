#include "git_repository.h"
#include "git.h"
#include "git_file.h"

#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QtConcurrent/QtConcurrent>

GitRepository::GitRepository(AppSettings &settings, QObject *parent)
    : QObject(parent), settings_(settings)
{
    git_ = new Git(this);

    watcher_ = new QFutureWatcher<CommandResult>(this);
    connect(watcher_,
            &QFutureWatcher<CommandResult>::progressValueChanged,
            this,
            &GitRepository::onFutureProgress);
    connect(watcher_, &QFutureWatcher<CommandResult>::canceled, this, [&]() {
        qDebug() << "Canceled";
        emit sgnReceived("Cancel", true);
    });
    connect(watcher_, &QFutureWatcher<CommandResult>::paused, this, []() { qDebug() << "Paused"; });
    connect(watcher_, &QFutureWatcher<CommandResult>::destroyed, this, []() {
        qDebug() << "Destroyed";
    });
    connect(watcher_, &QFutureWatcher<CommandResult>::finished, this, &GitRepository::onFinished);

    connect(watcher_,
            &QFutureWatcher<CommandResult>::resultReadyAt,
            this,
            &GitRepository::onResultReadyAt);

    connect(watcher_, &QFutureWatcher<CommandResult>::resumed, this, []() {
        qDebug() << "Resumed";
    });
    connect(watcher_, &QFutureWatcher<CommandResult>::started, this, &GitRepository::onStarted);
}

const QDir &GitRepository::getWorkingDir() const { return workingDir_; }

void GitRepository::setWorkingDir(const QDir &dir) { workingDir_ = dir; }

void GitRepository::status() {
  if (watcher_)
    watcher_->cancel();
  if (future_.isRunning())
      future_.cancel();

  currentCommand_ = GIT_COMMAND::STATUS;
  auto params = git_->makeStatusCommand();

  future_ = QtConcurrent::run(git_,
                              &Git::execute,
                              getWorkingDir().absolutePath(),
                              settings_.gitPath,
                              params);
  watcher_->setFuture(future_);

  emit sgnSended(QString("%1 %2").arg(settings_.gitPath).arg(params.join(" ")));
}

void GitRepository::commit(QString message, bool isAmend)
{
    if (watcher_)
        watcher_->cancel();

    if (future_.isRunning())
        future_.cancel();

    currentCommand_ = GIT_COMMAND::COMMIT;
    auto params = git_->makeCommitCommand(message, isAmend);

    future_ = QtConcurrent::run(git_,
                                &Git::execute,
                                getWorkingDir().absolutePath(),
                                settings_.gitPath,
                                params);
    watcher_->setFuture(future_);

    emit sgnSended(QString("%1 %2").arg(settings_.gitPath).arg(params.join(" ")));
}

void GitRepository::requestLastCommitMessage()
{
    if (watcher_)
        watcher_->cancel();
    if (future_.isRunning())
        future_.cancel();

    currentCommand_ = GIT_COMMAND::LAST_MESSAGE;
    auto params = git_->makeLastCommitMessageCommand();

    future_ = QtConcurrent::run(git_,
                                &Git::execute,
                                getWorkingDir().absolutePath(),
                                settings_.gitPath,
                                params);
    watcher_->setFuture(future_);
    emit sgnSended(QString("%1 %2").arg(settings_.gitPath).arg(params.join(" ")));
}

void GitRepository::readCurrentFile(QString filepath)
{
    auto dir = getWorkingDir();
    QFile currentFile(dir.absoluteFilePath(filepath));
    if (currentFile.exists() == false)
        return;

    currentFile.open(QIODevice::OpenModeFlag::ReadOnly);
    QString data = currentFile.readAll();
    currentFile.close();
    emit sgnCurrentFileReaded(filepath, data);
}

void GitRepository::readStagedOrCommitedFile(QString filepath)
{
    if (watcher_)
        watcher_->cancel();
    if (future_.isRunning())
        future_.cancel();

    currentCommand_ = GIT_COMMAND::SHOW;
    auto params = git_->makeShowCommand(filepath);

    future_ = QtConcurrent::run(git_,
                                &Git::execute,
                                getWorkingDir().absolutePath(),
                                settings_.gitPath,
                                params);
    watcher_->setFuture(future_);
    emit sgnSended(QString("%1 %2").arg(settings_.gitPath).arg(params.join(" ")));
}

void GitRepository::onStarted()
{
    qDebug() << "onStarted";
}

void GitRepository::onFutureProgress(int progressValue) {
  qDebug() << "onFutureProgress" << progressValue;
}
void GitRepository::onResultReadyAt(int resultIndex)
{
    qDebug() << "onResultReadyAt" << resultIndex;
    auto res = future_.resultAt(resultIndex);

    auto text = res.result.join("\n");
    emit sgnReceived(text, false);

    if (currentCommand_ == GIT_COMMAND::STATUS) {
        auto files = proccessGitStatus(text);
        emit sgnResultReceived(files);
    }
    if (currentCommand_ == GIT_COMMAND::COMMIT) {
    }
    if (currentCommand_ == GIT_COMMAND::LAST_MESSAGE) {
        proccessLastMessage(text);
    }
    if (currentCommand_ == GIT_COMMAND::SHOW) {
        proccessShowFileMessage(text);
    }

    currentCommand_ = GIT_COMMAND::NO;
}
void GitRepository::onFinished()
{
    qDebug() << "onFinished";
    //  currentCommand_ = GIT_COMMAND::NO;
    emit sgnFinished();
}

QVector<GitFile> GitRepository::proccessGitStatus(QString data) {
  auto lines = data.split("\n");

  QVector<GitFile> files;

  for (const auto &line : qAsConst(lines)) {
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

void GitRepository::proccessLastMessage(QString text)
{
    emit sgnLastMessageReady(text.mid(8));
}

void GitRepository::proccessShowFileMessage(QString data)
{
    emit sgnOriginalFileReaded(data);
}
