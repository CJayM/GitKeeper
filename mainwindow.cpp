#include "mainwindow.h"
#include "highlighter.h"
#include "settings_dialog.h"
#include "widgets/code_editor.h"

#include "domain/git_file.h"
#include "models/git_files_model.h"
#include "ui_mainwindow.h"

#include <QAction>
#include <QDebug>
#include <QDir>
#include <QScrollBar>
#include <QSettings>
#include <QSplashScreen>
#include <QTableView>

#include "domain/git_repository.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    settings_.load();
    restoreGeometry(settings_.geometry);
    ui->splitter_2->restoreState(settings_.splitter_2_state);
    ui->splitter_4->restoreState(settings_.splitter_4_state);
    ui->splitter->restoreState(settings_.splitter_state);

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

    QPixmap pixmap(":/resources/splash.png");
    splash_ = new QSplashScreen(this, pixmap);

    connect(ui->actionStatus, &QAction::triggered, this, &MainWindow::onStatusAction);
    connect(ui->actionCommit, &QAction::triggered, this, &MainWindow::onCommitAction);
    connect(ui->actionOpenSettings, &QAction::triggered, this, &MainWindow::onOpenSettingsDialog);
    connect(ui->aboutAction, &QAction::triggered, this, &MainWindow::onShowAbout);

    ui->btnCommit->setDefaultAction(ui->actionCommit);

    connect(ui->btnClearLog, &QAbstractButton::clicked, this, [&]() { ui->logMessage->clear(); });
    connect(ui->commitMessageEdit, &QTextEdit::textChanged, this, &MainWindow::onCommitTextChanged);
    connect(ui->amendCheckBox, &QCheckBox::toggled, this, &MainWindow::onAmnedChecked);

    highlighter_ = new Highlighter(ui->currentFileEdit->document());
    highlighterLeft_ = new Highlighter(ui->originalFileEdit->document());
    connect(ui->currentFileEdit->verticalScrollBar(),
            &QScrollBar::valueChanged,
            this,
            &MainWindow::onCurrentFileVScrollBarChanged);
    connect(ui->originalFileEdit->verticalScrollBar(),
            &QScrollBar::valueChanged,
            this,
            &MainWindow::onOriginalFileVScrollBarChanged);

    gitRepository_ = new GitRepository(settings_, this);
    gitRepository_->setWorkingDir(QDir("D:\\develop\\git_keeper\\GitKeeper"));
    connect(gitRepository_,
            &GitRepository::sgnResultReceived,
            this,
            &MainWindow::onGitStatusFinished);
    connect(gitRepository_, &GitRepository::sgnSended, this, &MainWindow::onSendedToGit);
    connect(gitRepository_, &GitRepository::sgnReceived, this, &MainWindow::onReceivedFromGit);
    connect(gitRepository_,
            &GitRepository::sgnLastMessageReady,
            this,
            &MainWindow::onReceivedLastMessage);
    connect(gitRepository_,
            &GitRepository::sgnCurrentFileReaded,
            this,
            &MainWindow::onCurrentFileReaded);
    connect(gitRepository_,
            &GitRepository::sgnOriginalFileReaded,
            this,
            &MainWindow::onOriginalFileReaded);
    connect(gitRepository_, &GitRepository::sgnDiffReaded, this, &MainWindow::onDiffReaded);

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
  if (current.isValid() == false) {
    ui->originalFileEdit->clear();
    ui->currentFileEdit->clear();
    return;
  }

  auto file = filesModel_->data(current.sibling(current.row(), 0), GitFilesModel::NAME_ROLE)
                  .toString();
  auto path = filesModel_->data(current.sibling(current.row(), 0), GitFilesModel::PATH_ROLE)
                  .toString();

  QDir dir(path);
  auto filepath = dir.filePath(file);
  gitRepository_->onSelectFile(filepath);
}

void MainWindow::onStatusAction() {  
  gitRepository_->status();
}

void MainWindow::onCommitAction() {
  auto message = ui->commitMessageEdit->toPlainText();
  gitRepository_->commit(message, ui->amendCheckBox->isChecked());

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

void MainWindow::onReceivedLastMessage(QString data)
{
    lastCommitMessage_ = data;
    ui->commitMessageEdit->setPlainText(data);
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

void MainWindow::onShowAbout()
{
    splash_->show();
    splash_->showMessage(QString("Version %1").arg(APP_VERSION), Qt::AlignLeft, Qt::white);
}

void MainWindow::onSaveSettings()
{
    settingsDialog_->saveSettings(settings_);
}

void MainWindow::onCommitTextChanged()
{
    if (stagedModel_->rowCount({}) == 0) {
        ui->btnCommit->setEnabled(false);
        return;
    }

    ui->btnCommit->setEnabled(ui->commitMessageEdit->toPlainText().isEmpty() == false);
}

void MainWindow::onAmnedChecked(bool checked)
{
    if (checked == false) {
        if (ui->commitMessageEdit->toPlainText() == lastCommitMessage_)
            ui->commitMessageEdit->clear();
        return;
    }

    gitRepository_->requestLastCommitMessage();
    //    ui->commitMessageEdit->setPlainText(lastCommitMessage_);
}

void MainWindow::onCurrentFileVScrollBarChanged(int value)
{
    qDebug() << "Scroll A" << value;
    ui->currentFileEdit->blockSignals(true);
    auto scrollBar = ui->originalFileEdit->verticalScrollBar();
    scrollBar->setValue(value);
    ui->currentFileEdit->blockSignals(false);
}

void MainWindow::onOriginalFileVScrollBarChanged(int value)
{
    qDebug() << "Scroll B" << value;
    ui->originalFileEdit->blockSignals(true);
    auto scrollBar = ui->currentFileEdit->verticalScrollBar();
    scrollBar->setValue(value);
    ui->originalFileEdit->blockSignals(false);
}

void MainWindow::onCurrentFileReaded(QString filepath, QString data)
{
    ui->currentFileEdit->setPlainText(data);
}

void MainWindow::onOriginalFileReaded(QString filepath, QString data)
{
    ui->originalFileEdit->setPlainText(data);
}

void MainWindow::onDiffReaded(QString filepath, QString data)
{
    diffs_.clear();

    for (const auto &line : data.split("\n"))
        qDebug() << line;
    for (const auto &line : data.split("\n")) {
        if (line.startsWith("@@") == false)
            continue;
        auto newLine = line.mid(4);
        auto parts = newLine.split("@@");
        auto digits = parts.first().split("+");
        auto leftParts = digits[0].trimmed().split(",");
        auto rightParts = digits[1].trimmed().split(",");

        if (leftParts.size() == 1)
            leftParts << "1";
        if (rightParts.size() == 1)
            rightParts << "1";

        DiffOperation oper;
        oper.left.line = leftParts[0].toInt();
        oper.left.count = leftParts[1].toInt();
        oper.right.line = rightParts[0].toInt();
        oper.right.count = rightParts[1].toInt();
        recognizeOperationType(oper);

        diffs_.append(oper);
    }

    colorize();
}

void MainWindow::colorize()
{
    QList<QTextEdit::ExtraSelection> selsRight;

    for (const auto &oper : qAsConst(diffs_.operations)) {
        ui->originalFileEdit->addDiffBlock(oper.left);
        ui->currentFileEdit->addDiffBlock(oper.right);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    settings_.geometry = saveGeometry();
    settings_.splitter_2_state = ui->splitter_2->saveState();
    settings_.splitter_4_state = ui->splitter_4->saveState();
    settings_.splitter_state = ui->splitter->saveState();
    settings_.save();

    QWidget::closeEvent(event);
}
