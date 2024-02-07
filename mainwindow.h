#pragma once

#include <QMainWindow>
#include <QModelIndex>
#include <QProcess>

#include <domain/git_file.h>

#include "domain/git.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QTableWidgetItem;
class QAction;
class GitFilesModel;
class GitRepository;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void onCurrentFileChanged(const QModelIndex &current,
                            const QModelIndex &previous);
  void onStatusAction();
  void onCommitAction();

  void onSendedToGit(QString data);
  void onReceivedFromGit(QString data, bool isError);
  void onGitStatusFinished(QVector<GitFile> files);

private:
  Ui::MainWindow *ui;

  GitFilesModel *filesModel_ = nullptr;
  GitRepository *gitRepository_ = nullptr;

  Git *git_ = nullptr;

  // QWidget interface
protected:
  void closeEvent(QCloseEvent *);
};
