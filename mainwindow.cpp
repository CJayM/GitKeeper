#include "mainwindow.h"
#include "highlighter.h"
#include "settings_dialog.h"

#include "domain/git_file.h"
#include "models/git_files_model.h"
#include "ui_mainwindow.h"

#include <QAction>
#include <QDebug>
#include <QDir>
#include <QScrollBar>
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
    connect(ui->actionOpenSettings, &QAction::triggered, this, &MainWindow::onOpenSettingsDialog);

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
    auto scrollBar = ui->originalFileEdit->verticalScrollBar();
    scrollBar->setValue(value);
}

void MainWindow::onCurrentFileReaded(QString filepath, QString data)
{
    ui->currentFileEdit->setText(data);
}

void MainWindow::onOriginalFileReaded(QString filepath, QString data)
{
    ui->originalFileEdit->setText(data);
}

void MainWindow::onDiffReaded(QString filepath, QString data)
{
    QStringList forLeft;
    QStringList forRight;
    for (const auto &line : data.split("\n"))
        qDebug() << line;
    for (const auto &line : data.split("\n")) {
        if (line.startsWith("@@") == false)
            continue;
        auto newLine = line.mid(4);
        auto parts = newLine.split("@@");
        auto digits = parts.first().split("+");
        forLeft << digits[0];
        forRight << digits[1];
    }

    fillLeft(forLeft);
    fillRight(forRight);
}

void MainWindow::fillLeft(const QStringList &texts)
{
    QList<QTextEdit::ExtraSelection> sels;
    auto document = ui->originalFileEdit->document();
    for (const auto &text : texts) {
        auto parts = text.trimmed().split(",");
        auto from = parts.first().toInt();
        int count = 1;
        if (parts.size() == 2)
            count = parts.at(1).toInt();

        for (int i = from - 1; i < from + count - 1; ++i) {
            QTextEdit::ExtraSelection selection;
            selection.format.setBackground(QBrush(QColor(Qt::yellow).lighter(160)));
            selection.format.setProperty(QTextFormat::FullWidthSelection, true);
            selection.cursor = QTextCursor(document);
            auto block = document->findBlockByLineNumber(i);
            selection.cursor.setPosition(block.position());
            selection.cursor.select(QTextCursor::LineUnderCursor);
            sels << selection;
        }
    }

    ui->originalFileEdit->setExtraSelections(sels);
}

void MainWindow::fillRight(const QStringList &texts)
{
    qDebug() << texts;
    QList<QTextEdit::ExtraSelection> sels;
    auto document = ui->currentFileEdit->document();
    for (const auto &text : texts) {
        auto parts = text.trimmed().split(",");
        auto from = parts.first().toInt();
        int count = 1;
        if (parts.size() == 2)
            count = parts.at(1).toInt();

        for (int i = from - 1; i < from + count - 1; ++i) {
            QTextEdit::ExtraSelection selection;
            selection.format.setBackground(QBrush(QColor(Qt::yellow).lighter(160)));
            selection.format.setProperty(QTextFormat::FullWidthSelection, true);
            selection.cursor = QTextCursor(document);
            auto block = document->findBlockByLineNumber(i);
            selection.cursor.setPosition(block.position());
            selection.cursor.select(QTextCursor::LineUnderCursor);
            sels << selection;
        }
    }

    ui->currentFileEdit->setExtraSelections(sels);
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
