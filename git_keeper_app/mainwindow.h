#pragma once

#include <QMainWindow>
#include <QModelIndex>
#include <QProcess>

#include "diff.h"
#include "domain/git.h"
#include "domain/git_file.h"
#include "domain/project.h"

#include "app_settings.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QTableWidgetItem;
class QAction;
class GitFilesModel;

class SettingsDialog;
class Highlighter;
class QSplashScreen;
class QItemSelectionModel;
class CodeEditor;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  private slots:
  void onCurrentFileIndexChanged(const QModelIndex &current, const QModelIndex &previous);
  void onStatusAction();
  void onCommitAction();

  void onSendedToGit(QString data);
  void onReceivedFromGit(QString data, bool isError);
  void onReceivedLastMessage(QString data);
  void onGitStatusFinished(QVector<GitFile> files);
  void onOpenSettingsDialog();
  void onShowAbout();
  void onPrevClick();
  void onNextClick();
  void onSaveSettings();
  void onCommitTextChanged();
  void onAmnedChecked(bool checked);
  void onCurrentFileVScrollBarChanged(int value);
  void onOriginalFileVScrollBarChanged(int value);
  void onCurrentFileChanged(QString path);
  void onCurrentFileReaded(QString filepath, QString before, QString after);
  void onDiffReaded();
  void onCurrentBlockChanged(QString filePath);
  void onHasPrevBlockChanged(bool hasPrev);
  void onHasNextBlockChanged(bool hasNext);
  void onAfterScrolledToBlock(int id);

  private:
  Ui::MainWindow *ui;

  GitFilesModel *filesModel_ = nullptr;
  GitFilesModel *stagedModel_ = nullptr;
  QString lastCommitMessage_ = "Amned text placed";
  QItemSelectionModel *selectionModel_ = nullptr;
  QString currentFilePath_;

  Git *git_ = nullptr;
  QSplashScreen *splash_ = nullptr;

  SettingsDialog *settingsDialog_ = nullptr;
  AppSettings settings_;
  Highlighter *highlighter_ = nullptr;
  Highlighter *highlighterLeft_ = nullptr;

  Project *diffs_ = nullptr;

  // QWidget interface
  protected:
  void closeEvent(QCloseEvent *);
};

void hightLightBlock(CodeEditor *widget, const DiffPos &diff);
