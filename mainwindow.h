#pragma once

#include <QMainWindow>
#include <QModelIndex>
#include <QProcess>

#include "diff.h"
#include "domain/git.h"
#include "domain/git_file.h"

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
class Highlighter;

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
  void onCurrentFileReaded(QString filepath, QString data);
  void onOriginalFileReaded(QString filepath, QString data);
  void onDiffReaded(QString filepath, QString data);

  private:
  Ui::MainWindow *ui;

  GitFilesModel *filesModel_ = nullptr;
  GitFilesModel *stagedModel_ = nullptr;
  GitRepository *gitRepository_ = nullptr;
  QString lastCommitMessage_ = "Amned text placed";

  Git *git_ = nullptr;

  SettingsDialog *settingsDialog_ = nullptr;
  AppSettings settings_;
  Highlighter *highlighter_ = nullptr;
  Highlighter *highlighterLeft_ = nullptr;

  void colorize();

  QVector<DiffOepration> operations_;

  // QWidget interface
protected:
  void closeEvent(QCloseEvent *);
};
