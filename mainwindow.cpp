#include "mainwindow.h"
#include "settings_dialog.h"

#include "domain/git_file.h"
#include "models/git_files_model.h"
#include "ui_mainwindow.h"

#include <QAction>
#include <QDebug>
#include <QDir>
#include <QSettings>
#include <QTableView>

#include <domain/git_repository.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    settings_.load();
    restoreGeometry(settings_.geometry);
    ui->splitter_2->restoreState(settings_.splitter_2_state);
    ui->splitter_4->restoreState(settings_.splitter_4_state);
    ui->splitter->restoreState(settings_.splitter_state);
    ui->splitter_3->restoreState(settings_.splitter_3_state);

    settingsDialog_ = new SettingsDialog(this);
    settingsDialog_->setModal(true);
    connect(settingsDialog_, &QDialog::accepted, this, &MainWindow::onSaveSettings);

    filesModel_ = new GitFilesModel();
    ui->filesTableView->setModel(filesModel_);

    stagedModel_ = new GitFilesModel();
    stagedModel_->setIsStaged(true);
    ui->stagedTableView->setModel(stagedModel_);

    auto selectionModel = ui->filesTableView->selectionModel();

    connect(selectionModel,
            &QItemSelectionModel::currentChanged,
            this,
            &MainWindow::onCurrentFileChanged);

    connect(ui->actionStatus, &QAction::triggered, this, &MainWindow::onStatusAction);
    connect(ui->actionCommit, &QAction::triggered, this, &MainWindow::onCommitAction);

    ui->btnCommit->setDefaultAction(ui->actionCommit);

    connect(ui->btnClearLog, &QAbstractButton::clicked, this, [&]() { ui->logMessage->clear(); });

    gitRepository_ = new GitRepository(settings_, this);
    gitRepository_->setWorkingDir(QDir("D:\\develop\\git_keeper\\GitKeeper"));
    connect(gitRepository_,
            &GitRepository::sgnResultReceived,
            this,
            &MainWindow::onGitStatusFinished);
    connect(gitRepository_, &GitRepository::sgnSended, this, &MainWindow::onSendedToGit);
    connect(gitRepository_, &GitRepository::sgnReceived, this, &MainWindow::onReceivedFromGit);

    connect(ui->actionOpenSettings, &QAction::triggered, this, &MainWindow::onOpenSettingsDialog);
    gitRepository_->status();
}

MainWindow::~MainWindow()
{
    settingsDialog_->deleteLater();
    settingsDialog_ = nullptr;

    delete ui;
}

void MainWindow::onCurrentFileChanged(const QModelIndex &current,
                                      const QModelIndex &previous) {
  qDebug() << "Changed" << current;
  if (current.isValid() == false) {
    ui->originalFileEdit->clear();
    ui->currentFileEdit->clear();
    return;
  }

  auto file =
      filesModel_->data(current.sibling(current.row(), 0), Qt::DisplayRole)
          .toString();
  ui->currentFileEdit->setText(file);
}

void MainWindow::onStatusAction() {  
  gitRepository_->status();
}

void MainWindow::onCommitAction() {
  auto message = ui->commitMessageEdit->toPlainText();
  gitRepository_->commit(message);

  ui->commitMessageEdit->clear();
}

void MainWindow::onSendedToGit(QString data) {
  ui->logMessage->append(QString(">><b>%1</b>").arg(data));
}

void MainWindow::onReceivedFromGit(QString data, bool isError) {
  if (isError)
    ui->logMessage->append(QString("ERROR:<i>%1</i>").arg(data));
  else
    ui->logMessage->append(QString("<span>%1</span>").arg(data));
}

void MainWindow::onGitStatusFinished(QVector<GitFile> files)
{
    filesModel_->setFiles(files);
    stagedModel_->setFiles(files);
}

void MainWindow::onOpenSettingsDialog()
{
    settingsDialog_->loadSettings(settings_);
    settingsDialog_->show();
}

void MainWindow::onSaveSettings()
{
    settingsDialog_->saveSettings(settings_);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    settings_.geometry = saveGeometry();
    settings_.splitter_2_state = ui->splitter_2->saveState();
    settings_.splitter_4_state = ui->splitter_4->saveState();
    settings_.splitter_state = ui->splitter->saveState();
    settings_.splitter_3_state = ui->splitter_3->saveState();
    settings_.save();

    QWidget::closeEvent(event);
}
