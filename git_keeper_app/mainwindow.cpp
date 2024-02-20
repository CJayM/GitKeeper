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
    connect(ui->prevFileChangeAction, &QAction::triggered, this, &MainWindow::onPrevChange);
    connect(ui->nextFileChangeAction, &QAction::triggered, this, &MainWindow::onNextChange);

    ui->btnCommit->setDefaultAction(ui->actionCommit);
    ui->prevFileChangeBtn->setDefaultAction(ui->prevFileChangeAction);
    ui->nextFileChangeBtn->setDefaultAction(ui->nextFileChangeAction);

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

    diffs_ = new Diffs(settings_.gitPath, this);
    diffs_->setPath(QDir("D:\\develop\\git_keeper\\GitKeeper"));

    connect(diffs_, &Diffs::sgnFilesChanged, this, &MainWindow::onGitStatusFinished);
    connect(diffs_, &Diffs::sgnSendedToGit, this, &MainWindow::onSendedToGit);
    connect(diffs_, &Diffs::sgnReceivedFromGit, this, &MainWindow::onReceivedFromGit);
    connect(diffs_, &Diffs::sgnLastMessageProcessed, this, &MainWindow::onReceivedLastMessage);
    connect(diffs_, &Diffs::sgnAfterChanged, this, &MainWindow::onCurrentFileReaded);
    connect(diffs_, &Diffs::sgnBeforeChanged, this, &MainWindow::onOriginalFileReaded);
    connect(diffs_, &Diffs::sgnDiffChanged, this, &MainWindow::onDiffReaded);
    diffs_->status();

    connect(ui->prevFileBtn, &QAbstractButton::clicked, this, &MainWindow::onPrevFileClicked);
    connect(ui->nextFileBtn, &QAbstractButton::clicked, this, &MainWindow::onNextFileClicked);
}

MainWindow::~MainWindow()
{
    settingsDialog_->deleteLater();
    settingsDialog_ = nullptr;

    delete ui;
}

void MainWindow::onCurrentFileChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if (current.isValid() == false) {
        ui->originalFileEdit->clear();
        ui->currentFileEdit->clear();
        return;
    }

    auto filepath = filesModel_
                        ->data(current.sibling(current.row(), 0), GitFilesModel::FULL_PATH_ROLE)
                        .toString();
    diffs_->selectCurrentFile(filepath);
}

void MainWindow::onStatusAction()
{
    diffs_->status();
}

void MainWindow::onCommitAction() {
  auto message = ui->commitMessageEdit->toPlainText();
  diffs_->commit(message, ui->amendCheckBox->isChecked());
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

void MainWindow::onPrevChange()
{
    ui->nextFileChangeAction->setEnabled(true);
    auto oper = diffs_->getPrevChange();
    if (oper.right.type == DiffOperationType::UNINITIALIZED) {
        ui->prevFileChangeAction->setEnabled(false);
        qDebug() << "End changes";
        return;
    }
    ui->prevFileChangeAction->setEnabled(true);

    qDebug() << "Scroll to " << oper.right.line;
    auto block = ui->currentFileEdit->document()->findBlockByLineNumber(oper.right.line - 1);
    ui->currentFileEdit->setTextCursor(QTextCursor(block));
}

void MainWindow::onNextChange()
{
    ui->prevFileChangeAction->setEnabled(true);

    auto oper = diffs_->getNextChange();
    if (oper.right.type == DiffOperationType::UNINITIALIZED) {
        qDebug() << "End changes";
        ui->nextFileChangeAction->setEnabled(false);
        return;
    }
    ui->nextFileChangeAction->setEnabled(true);

    qDebug() << "Scroll to " << oper.right.line;
    auto block = ui->currentFileEdit->document()->findBlockByLineNumber(oper.right.line - 1);
    ui->currentFileEdit->setTextCursor(QTextCursor(block));
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

    diffs_->queryLastCommitMessage();
    //    ui->commitMessageEdit->setPlainText(lastCommitMessage_);
}

void MainWindow::onCurrentFileVScrollBarChanged(int value)
{
    qDebug() << "Scroll B" << value;
    ui->currentFileEdit->blockSignals(true);
    auto mappedPos = diffs_->getMappedLeftPos(value);
    auto scrollBar = ui->originalFileEdit->verticalScrollBar();
    scrollBar->setValue(mappedPos);
    ui->currentFileEdit->blockSignals(false);
}

void MainWindow::onOriginalFileVScrollBarChanged(int value)
{
    qDebug() << "Scroll A" << value;
    ui->originalFileEdit->blockSignals(true);
    auto mappedPos = diffs_->getMappedRightPos(value);
    auto scrollBar = ui->currentFileEdit->verticalScrollBar();
    scrollBar->setValue(mappedPos);
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

void MainWindow::onDiffReaded()
{
    ui->originalFileEdit->clearDiffBlocks();
    ui->currentFileEdit->clearDiffBlocks();

    for (const auto &oper : qAsConst(diffs_->operations)) {
        ui->originalFileEdit->addDiffBlock(oper.left);
        ui->currentFileEdit->addDiffBlock(oper.right);
    }
}

void MainWindow::onPrevFileClicked()
{
    diffs_->selectPrevFile();
}

void MainWindow::onNextFileClicked()
{
    diffs_->selectNextFile();
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
