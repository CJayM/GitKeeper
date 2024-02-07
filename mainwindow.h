#pragma once

#include <QMainWindow>
#include <QModelIndex>
#include <QProcess>

#include <domain/git_file.h>

#include "domain/git.h"

#include "app_settings.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QTableWidgetItem;
class QAction;
class GitFilesModel;
class GitRepository;

class SettingsDialog;

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
  void onOpenSettingsDialog();
  void onSaveSettings();

  private:
  Ui::MainWindow *ui;

  GitFilesModel *filesModel_ = nullptr;
  GitFilesModel *stagedModel_ = nullptr;
  GitRepository *gitRepository_ = nullptr;

  Git *git_ = nullptr;

  SettingsDialog *settingsDialog_ = nullptr;
  AppSettings settings_;

  // QWidget interface
protected:
  void closeEvent(QCloseEvent *);
};
