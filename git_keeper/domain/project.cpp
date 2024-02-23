#include "project.h"
#include "git_repository.h"

Project::Project(QString gitPath, QObject *parent) : QObject(parent), gitPath_(gitPath)
{
    gitRepository_ = new GitRepository(gitPath_, this);

    connect(gitRepository_, &GitRepository::sgnResultReceived, this, &Project::onGitStatusFinished);
    connect(gitRepository_, &GitRepository::sgnSended, this, &Project::onSendedToGit);
    connect(gitRepository_, &GitRepository::sgnReceived, this, &Project::onReceivedFromGit);
    connect(gitRepository_,
            &GitRepository::sgnLastMessageReady,
            this,
            &Project::onReceivedLastMessage);
    connect(gitRepository_,
            &GitRepository::sgnCurrentFuleContentReaded,
            this,
            &Project::onCurrentFileReaded);
    connect(gitRepository_,
            &GitRepository::sgnCurrentFuleContentReaded,
            this,
            &Project::onCurrentFileReaded);
    connect(gitRepository_, &GitRepository::sgnDiffsReaded, this, &Project::onDiffsReaded);
}

void Project::setGitPath(QString path)
{
    gitPath_ = path;
    gitRepository_->setGitPath(path);
}

void Project::setPath(QDir path)
{
    gitRepository_->setWorkingDir(path);
}

void Project::clearDiffs()
{
    for (auto &arr : operations_.values()) {
        for (auto &oper : arr)
            delete oper;
        arr.clear();
    }

    operations_.clear();
    operationsList_.clear();
    currentOperationIndex_ = -1;
}

void Project::addDiff(DiffOperation *oper)
{
    if (oper == nullptr)
        return;

    if (operations_.contains(oper->filePath) == false)
        operations_[oper->filePath] = {};

    operations_[oper->filePath].append(oper);
    operationsList_.append(operations_[oper->filePath].last());
}

int Project::getMappedLeftPos(int pos) const
{
    return pos;
    //    if (operations.isEmpty())
    //        return pos;

    //    const DiffOperation *minOper = &operations.first();
    //    const DiffOperation *maxOper = nullptr;

    //    for (const auto &oper : operations) {
    //        maxOper = &oper;
    //        if (maxOper->right.line >= pos)
    //            break;
    //        minOper = maxOper;
    //    }

    //    if (minOper == maxOper)
    //        return pos;

    //    float delta = maxOper->right.line - minOper->right.line;
    //    float percent = (pos - minOper->right.line) / delta;

    //    float delta2 = maxOper->left.line - minOper->left.line;
    //    float result = minOper->left.line + int((delta2 * percent));

    //    return result;
}

int Project::getMappedRightPos(int pos) const
{
    return pos;
    //    if (operations.isEmpty())
    //        return pos;

    //    const DiffOperation *minOper = &operations.first();
    //    const DiffOperation *maxOper = nullptr;

    //    for (const auto &oper : operations) {
    //        maxOper = &oper;
    //        if (maxOper->left.line >= pos)
    //            break;
    //        minOper = maxOper;
    //    }

    //    if (minOper == maxOper)
    //        return pos;

    //    float delta = maxOper->left.line - minOper->left.line;
    //    float percent = (pos - minOper->left.line) / delta;

    //    float delta2 = maxOper->right.line - minOper->right.line;
    //    float result = minOper->right.line + int((delta2 * percent));

    //    return result;
}

const QVector<DiffOperation *> Project::getCurrentFileDiffs() const
{
    return operations_[currentFile_];
}

DiffOperation *Project::getCurrentBlock()
{
    if (operationsList_.isEmpty())
        return nullptr;
    if (currentOperationIndex_ >= operationsList_.size())
        return nullptr;

    if (currentOperationIndex_ == -1)
        return nullptr;

    return operationsList_[currentOperationIndex_];
}

void Project::setBlockForFile(const QString &filePath)
{
    if (operations_.contains(filePath) == false)
        return;

    auto block = operations_[filePath].first();
    currentOperationIndex_ = operationsList_.indexOf(block);
}

void Project::movePrevChange()
{
    if (operationsList_.isEmpty()) {
        emit sgnHasNextBlockChanged(false);
        emit sgnHasPrevBlockChanged(false);
        return;
    }

    if (currentOperationIndex_ == 0)
        return;

    setCurrentBlock(currentOperationIndex_ != -1 ? currentOperationIndex_ - 1 : 0);
}

void Project::moveNextChange()
{
    if (operationsList_.isEmpty()) {
        emit sgnHasNextBlockChanged(false);
        emit sgnHasPrevBlockChanged(false);
        return;
    }

    if (currentOperationIndex_ == (operationsList_.size() - 1))
        return;

    setCurrentBlock(currentOperationIndex_ != -1 ? currentOperationIndex_ + 1 : 0);
}

void Project::setCurrentBlock(int id)
{
    QString oldPath;
    if (currentOperationIndex_ != -1)
        oldPath = operationsList_[currentOperationIndex_]->filePath;

    if (currentOperationIndex_ == -1)
        currentOperationIndex_ = 0;
    else
        currentOperationIndex_ = id;

    auto oper = operationsList_[currentOperationIndex_];
    emit sgnCurrentBlockChanged(oper->filePath);

    if (oper->filePath != oldPath)
        selectCurrentFile(oper->filePath);

    if (currentOperationIndex_ == 0)
        emit sgnHasPrevBlockChanged(false);
    else
        emit sgnHasPrevBlockChanged(true);

    if (currentOperationIndex_ == (operationsList_.size() - 1))
        emit sgnHasNextBlockChanged(false);
    else
        emit sgnHasNextBlockChanged(true);
}

void Project::clearCurrentBlockIndex()
{
    currentOperationIndex_ = -1;
}

void Project::selectCurrentFile(QString filepath)
{
    if (currentFile_ == filepath)
        return;

    currentFile_ = filepath;
    gitRepository_->queryFile(filepath);
    emit sgnCurrentFileChanged(filepath);
}

void Project::status()
{
    gitRepository_->status();
}

void Project::commit(QString message, bool isAmned)
{
    gitRepository_->commit(message, isAmned);
}

void Project::queryLastCommitMessage()
{
    gitRepository_->requestLastCommitMessage();
}

void Project::onGitStatusFinished(QVector<GitFile> result)
{
    changedFiles_ = result;
    emit sgnFilesChanged(changedFiles_);
}

void Project::onSendedToGit(QString data)
{
    emit sgnSendedToGit(data);
}

void Project::onReceivedFromGit(QString data, bool isError)
{
    emit sgnReceivedFromGit(data, isError);
}

void Project::onReceivedLastMessage(QString data)
{
    emit sgnLastMessageProcessed(data);
}

void Project::onCurrentFileReaded(QString filepath, QString before, QString after)
{
    emit sgnCurrentContentReloaded(filepath, before, after);
}

void Project::onDiffsReaded(QStringList data)
{
    clearDiffs();

    QString filePath;
    int id = 0;
    for (const auto &line : data) {
        if (line.startsWith("+++ b/")) {
            filePath = line.mid(6);
            continue;
        }

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

        auto oper = new DiffOperation();
        oper->filePath = filePath;
        oper->left.id = id;
        oper->left.line = leftParts[0].toInt();
        oper->left.count = leftParts[1].toInt();
        oper->right.id = id;
        oper->right.line = rightParts[0].toInt();
        oper->right.count = rightParts[1].toInt();
        recognizeOperationType(oper);

        addDiff(oper); // todo: удалить эту команду (она приватная)
        ++id;
    }

    emit sgnDiffsReloaded();
}
