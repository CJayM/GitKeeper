#include "mainwindow.h"

#include "domain/git_file.h"
#include "models/git_files_model.h"
#include "ui_mainwindow.h"

#include <QAction>
#include <QDebug>
#include <QDir>
#include <QSettings>
#include <QTableView>

#include <domain/git_repository.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  QSettings settings("kb23", "GitKeeper");
  restoreGeometry(settings.value("geometry").toByteArray());

  filesModel_ = new GitFilesModel();
  ui->filesTableView->setModel(filesModel_);
  ui->stagedTableView->setModel(filesModel_);

  auto selectionModel = ui->filesTableView->selectionModel();

  connect(selectionModel, &QItemSelectionModel::currentChanged, this,
          &MainWindow::onCurrentFileChanged);

  connect(ui->actionStatus, &QAction::triggered, this, &MainWindow::onStatusAction);

  connect(ui->actionCommit, &QAction::triggered, this, &MainWindow::onCommitAction);
  ui->btnCommit->setDefaultAction(ui->actionCommit);

  gitRepository_ = new GitRepository(this);
  gitRepository_->setWorkingDir(QDir("D:\\develop\\git_keeper\\GitKeeper"));
  connect(gitRepository_, &GitRepository::sgnResultReceived, this,
          &MainWindow::onGitStatusFinished);
  connect(gitRepository_, &GitRepository::sgnSended, this,
          &MainWindow::onSendedToGit);
  connect(gitRepository_, &GitRepository::sgnReceived, this,
          &MainWindow::onReceivedFromGit);

  gitRepository_->status();
}

MainWindow::~MainWindow() { delete ui; }

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
  filesModel_->setFiles({});
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

void MainWindow::onGitStatusFinished(QVector<GitFile> files) {
  filesModel_->setFiles(files);
}

void MainWindow::closeEvent(QCloseEvent *event) {
  QSettings settings("kb23", "GitKeeper");
  settings.setValue("geometry", saveGeometry());
  QWidget::closeEvent(event);
}
