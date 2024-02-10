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
  void onReceivedLastMessage(QString data);
  void onGitStatusFinished(QVector<GitFile> files);
  void onOpenSettingsDialog();
  void onSaveSettings();
  void onCommitTextChanged();
  void onAmnedChecked(bool checked);
  void onCurrentFileVScrollBarChanged(int value);

  private:
  Ui::MainWindow *ui;

  GitFilesModel *filesModel_ = nullptr;
  GitFilesModel *stagedModel_ = nullptr;
  GitRepository *gitRepository_ = nullptr;
  QString lastCommitMessage_ = "Amned text placed";

  Git *git_ = nullptr;

  SettingsDialog *settingsDialog_ = nullptr;
  AppSettings settings_;

  // QWidget interface
protected:
  void closeEvent(QCloseEvent *);
};
