#include "mainwindow.h"
#include "highlighter.h"
#include "settings_dialog.h"
#include "widgets/code_editor.h"

#include "app_palette.h"
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

const char *SELECTION_CHANGED_FROM_CODE = "SELECTION_CHANGED_FROM_CODE";

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

    selectionModel_ = ui->filesTableView->selectionModel();
    connect(selectionModel_,
            &QItemSelectionModel::currentChanged,
            this,
            &MainWindow::onCurrentFileIndexChanged,
            Qt::DirectConnection);

    QPixmap pixmap(":/resources/splash.png");
    splash_ = new QSplashScreen(this, pixmap);

    connect(ui->actionStatus, &QAction::triggered, this, &MainWindow::onStatusAction);
    connect(ui->actionCommit, &QAction::triggered, this, &MainWindow::onCommitAction);
    connect(ui->actionOpenSettings, &QAction::triggered, this, &MainWindow::onOpenSettingsDialog);
    connect(ui->aboutAction, &QAction::triggered, this, &MainWindow::onShowAbout);
    connect(ui->prevBlockChangeAction, &QAction::triggered, this, &MainWindow::onPrevClick);
    connect(ui->nextBlockChangeAction, &QAction::triggered, this, &MainWindow::onNextClick);

    ui->btnCommit->setDefaultAction(ui->actionCommit);
    ui->prevBlockBtn->setDefaultAction(ui->prevBlockChangeAction);
    ui->nextBlockBtn->setDefaultAction(ui->nextBlockChangeAction);

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

    diffs_ = new Project(settings_.gitPath, this);
    diffs_->setPath(QDir("D:\\develop\\git_keeper\\GitKeeper"));

    connect(diffs_, &Project::sgnFilesChanged, this, &MainWindow::onGitStatusFinished);
    connect(diffs_, &Project::sgnSendedToGit, this, &MainWindow::onSendedToGit);
    connect(diffs_, &Project::sgnReceivedFromGit, this, &MainWindow::onReceivedFromGit);
    connect(diffs_, &Project::sgnLastMessageProcessed, this, &MainWindow::onReceivedLastMessage);
    connect(diffs_, &Project::sgnCurrentContentReloaded, this, &MainWindow::onCurrentFileReaded);
    connect(diffs_, &Project::sgnDiffsReloaded, this, &MainWindow::onDiffReaded);
    connect(diffs_, &Project::sgnCurrentFileChanged, this, &MainWindow::onCurrentFileChanged);
    connect(diffs_, &Project::sgnCurrentBlockChanged, this, &MainWindow::onCurrentBlockChanged);
    connect(diffs_, &Project::sgnHasPrevBlockChanged, this, &MainWindow::onHasPrevBlockChanged);
    connect(diffs_, &Project::sgnHasNextBlockChanged, this, &MainWindow::onHasNextBlockChanged);

    diffs_->status();
}

MainWindow::~MainWindow()
{
    settingsDialog_->deleteLater();
    settingsDialog_ = nullptr;

    delete ui;
}

void MainWindow::onCurrentFileIndexChanged(const QModelIndex &current, const QModelIndex &previous)
{
    auto isFromCode = selectionModel_->property(SELECTION_CHANGED_FROM_CODE).toBool();
    if (isFromCode)
        return;

    if (current.isValid() == false) {
        ui->originalFileEdit->clear();
        ui->currentFileEdit->clear();
        return;
    }

    auto filepath = filesModel_
                        ->data(current.sibling(current.row(), 0), GitFilesModel::FULL_PATH_ROLE)
                        .toString();

    diffs_->clearCurrentBlockIndex();
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

void MainWindow::onPrevClick()
{
    diffs_->movePrevChange();
}

void MainWindow::onNextClick()
{
    diffs_->moveNextChange();
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

void MainWindow::onCurrentFileChanged(QString path)
{
    qDebug() << "Change current filer" << path;
    if (path != currentFilePath_) {
        currentFilePath_ = path;
        onCurrentBlockChanged(path);

        qDebug() << "Color diffs";

        ui->originalFileEdit->clearDiffBlocks();
        ui->currentFileEdit->clearDiffBlocks();

        const auto diffs = diffs_->getCurrentFileDiffs();

        for (const auto &oper : diffs) {
            ui->originalFileEdit->addDiffBlock(oper->left);
            ui->currentFileEdit->addDiffBlock(oper->right);
        }
    }
    auto indexes = filesModel_->match(filesModel_->index(0, 0, {}),
                                      GitFilesModel::FULL_PATH_ROLE,
                                      path,

                                      1);
    selectionModel_->setProperty(SELECTION_CHANGED_FROM_CODE, true);
    if (indexes.isEmpty()) {
        selectionModel_->setCurrentIndex({}, QItemSelectionModel::SelectionFlag::Clear);
        selectionModel_->setProperty(SELECTION_CHANGED_FROM_CODE, false);
        return;
    }
    selectionModel_->setProperty(SELECTION_CHANGED_FROM_CODE, true);
    selectionModel_->setCurrentIndex(indexes.first(),
                                     QItemSelectionModel::SelectionFlag::ClearAndSelect
                                         | QItemSelectionModel::SelectionFlag::Current
                                         | QItemSelectionModel::SelectionFlag::Rows);
    selectionModel_->setProperty(SELECTION_CHANGED_FROM_CODE, false);
}

void MainWindow::onCurrentBlockChanged(QString filePath)
{
    if ((currentFilePath_.isEmpty() == false) && (currentFilePath_ != filePath))
        return;

    auto oper = diffs_->getCurrentBlock();
    if (oper == nullptr) {
        diffs_->setBlockForFile(filePath);
        oper = diffs_->getCurrentBlock();
        Q_ASSERT(oper != nullptr);
    }

    hightLightBlock(ui->originalFileEdit, oper->left);
    hightLightBlock(ui->currentFileEdit, oper->right);
}

void MainWindow::onCurrentFileReaded(QString filepath, QString before, QString after)
{
    ui->originalFileEdit->setPlainText(before);
    ui->currentFileEdit->setPlainText(after);
    onCurrentBlockChanged(filepath);
}

void MainWindow::onDiffReaded()
{
    qDebug() << "onDiffReaded";
    diffs_->moveNextChange();
}

void MainWindow::onHasPrevBlockChanged(bool hasPrev)
{
    ui->prevBlockChangeAction->setEnabled(hasPrev);
}

void MainWindow::onHasNextBlockChanged(bool hasNext)
{
    ui->nextBlockChangeAction->setEnabled(hasNext);
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

void hightLightBlock(CodeEditor *widget, const DiffPos &diff)
{
    const auto &document = widget->document();
    auto block = document->findBlockByLineNumber(diff.line - 1);
    if (block.isValid() == false)
        return;

    widget->setCurrentBlock(diff);
    widget->setTextCursor(QTextCursor(block));
    if (diff.count == 0)
        return;

    //    onOriginalFileVScrollBarChanged(oper->right.line - 1);
    auto end = document->findBlockByLineNumber(diff.line - 1 + diff.count - 1);

    QList<QTextEdit::ExtraSelection> extraSelections;
    auto pos = block.position();
    QTextEdit::ExtraSelection selection;
    selection.cursor = QTextCursor(document);
    selection.cursor.setPosition(pos);
    auto endPos = end.position() + end.length() - 1;
    selection.cursor.setPosition(endPos, QTextCursor::KeepAnchor);

    qDebug() << "FILL" << diff.id << diff.line << diff.count << static_cast<int>(diff.type);

    if (diff.type == DiffOperationType::ADD)
        selection.format.setBackground(AppPalette::DiffAddedLineExtraSelectionBrush);

    if (diff.type == DiffOperationType::REMOVE)
        selection.format.setBackground(AppPalette::DiffRemovedLineExtraSelectionBrush);

    if (diff.type == DiffOperationType::REPLACE)
        selection.format.setBackground(AppPalette::DiffUpdatedLineExtraSelectionBrush);

    if (diff.type == DiffOperationType::NOTHINK)
        selection.format.setBackground(AppPalette::DiffNothinkLineExtraSelectionBrush);

    extraSelections.append(selection);
    widget->setExtraSelections(extraSelections);
}
