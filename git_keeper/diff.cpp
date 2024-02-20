#include "diff.h"

#include "domain/git_repository.h"

#include <QDebug>

void recognizeOperationType(DiffOperation &oper)
{
    if (oper.left.count > 0 && oper.right.count == 0) {
        oper.left.type = DiffOperationType::REMOVE;
        oper.right.type = DiffOperationType::REMOVE;
        return;
    }

    if (oper.left.count == 0 && oper.right.count > 0) {
        oper.left.type = DiffOperationType::ADD;
        oper.right.type = DiffOperationType::ADD;
        return;
    }

    oper.left.type = DiffOperationType::REPLACE;
    oper.right.type = DiffOperationType::ADD;
    return;
}

Diffs::Diffs(QString gitPath, QObject *parent) : QObject(parent), gitPath_(gitPath)
{
    gitRepository_ = new GitRepository(gitPath_, this);

    connect(gitRepository_, &GitRepository::sgnResultReceived, this, &Diffs::onGitStatusFinished);
    connect(gitRepository_, &GitRepository::sgnSended, this, &Diffs::onSendedToGit);
    connect(gitRepository_, &GitRepository::sgnReceived, this, &Diffs::onReceivedFromGit);
    connect(gitRepository_,
            &GitRepository::sgnLastMessageReady,
            this,
            &Diffs::onReceivedLastMessage);
    connect(gitRepository_, &GitRepository::sgnCurrentFileReaded, this, &Diffs::onCurrentFileReaded);
    connect(gitRepository_,
            &GitRepository::sgnOriginalFileReaded,
            this,
            &Diffs::onOriginalFileReaded);
    connect(gitRepository_, &GitRepository::sgnDiffReaded, this, &Diffs::onDiffReaded);
}

void Diffs::setGitPath(QString path)
{
    gitPath_ = path;
    gitRepository_->setGitPath(path);
}

void Diffs::setPath(QDir path)
{
    gitRepository_->setWorkingDir(path);
}

void Diffs::clear()
{
    operations.clear();
    currentOperationIndex = -1;
}

void Diffs::append(DiffOperation oper)
{
    operations.append(oper);
}

int Diffs::getMappedLeftPos(int pos) const
{
    if (operations.isEmpty())
        return pos;

    const DiffOperation *minOper = &operations.first();
    const DiffOperation *maxOper = nullptr;

    for (const auto &oper : operations) {
        maxOper = &oper;
        if (maxOper->right.line >= pos)
            break;
        minOper = maxOper;
    }

    if (minOper == maxOper)
        return pos;

    float delta = maxOper->right.line - minOper->right.line;
    float percent = (pos - minOper->right.line) / delta;

    float delta2 = maxOper->left.line - minOper->left.line;
    float result = minOper->left.line + int((delta2 * percent));

    return result;
}

int Diffs::getMappedRightPos(int pos) const
{
    if (operations.isEmpty())
        return pos;

    const DiffOperation *minOper = &operations.first();
    const DiffOperation *maxOper = nullptr;

    for (const auto &oper : operations) {
        maxOper = &oper;
        if (maxOper->left.line >= pos)
            break;
        minOper = maxOper;
    }

    if (minOper == maxOper)
        return pos;

    float delta = maxOper->left.line - minOper->left.line;
    float percent = (pos - minOper->left.line) / delta;

    float delta2 = maxOper->right.line - minOper->right.line;
    float result = minOper->right.line + int((delta2 * percent));

    return result;
}

DiffOperation Diffs::getPrevChange()
{
    currentOperationIndex -= 1;
    if (currentOperationIndex < 0) {
        if (hasPrevFile()) {
            selectPrevFile();
            currentOperationIndex = -1;
            return getNextChange();
        }
        return {};
    }

    return operations[currentOperationIndex];
}

DiffOperation Diffs::getNextChange()
{
    currentOperationIndex += 1;
    if (currentOperationIndex >= operations.size()) {
        if (hasNextFile()) {
            selectNextFile();
            currentOperationIndex = -1;
            return getNextChange();
        }

        return {};
    }

    return operations[currentOperationIndex];
}

void Diffs::selectCurrentFile(QString filepath)
{
    currentFile_ = filepath;
    qDebug() << "Current file:" << filepath;
    gitRepository_->queryFile(filepath);
    emit sgnCurrentFileChanged(filepath);
}

bool Diffs::hasNextFile() const
{
    if (changedFiles_.isEmpty())
        return false;

    bool finded = false;
    for (const auto &file : changedFiles_) {
        auto fullPath = file.fullPath();
        if (finded) {
            return true;
        }
        if (fullPath == currentFile_) {
            finded = true;
            continue;
        }
    }

    return false;
}

bool Diffs::hasPrevFile() const
{
    if (changedFiles_.isEmpty())
        return false;

    QString prevFile;
    for (const auto &file : changedFiles_) {
        auto fullPath = file.fullPath();
        if (fullPath == currentFile_) {
            if (prevFile.isEmpty() == false) {
                return true;
            }
            break;
        }
        prevFile = fullPath;
    }

    return false;
}

bool Diffs::selectNextFile()
{
    bool finded = false;

    for (const auto &file : changedFiles_) {
        auto fullPath = file.fullPath();
        if (finded) {
            selectCurrentFile(fullPath);
            return true;
        }
        if (fullPath == currentFile_) {
            finded = true;
            continue;
        }
    }

    return false;
}

bool Diffs::selectPrevFile()
{
    QString prevFile;
    for (const auto &file : changedFiles_) {
        auto fullPath = file.fullPath();
        if (fullPath == currentFile_) {
            if (prevFile.isEmpty() == false) {
                selectCurrentFile(prevFile);
                return true;
            }
            break;
        }
        prevFile = fullPath;
    }

    return false;
}

void Diffs::status()
{
    gitRepository_->status();
}

void Diffs::commit(QString message, bool isAmned)
{
    gitRepository_->commit(message, isAmned);
}

void Diffs::queryLastCommitMessage()
{
    gitRepository_->requestLastCommitMessage();
}

void Diffs::onGitStatusFinished(QVector<GitFile> result)
{
    changedFiles_ = result;
    emit sgnFilesChanged(changedFiles_);
}

void Diffs::onSendedToGit(QString data)
{
    emit sgnSendedToGit(data);
}

void Diffs::onReceivedFromGit(QString data, bool isError)
{
    emit sgnReceivedFromGit(data, isError);
}

void Diffs::onReceivedLastMessage(QString data)
{
    emit sgnLastMessageProcessed(data);
}

void Diffs::onCurrentFileReaded(QString filepath, QString data)
{
    emit sgnAfterChanged(filepath, data);
}

void Diffs::onOriginalFileReaded(QString filepath, QString data)
{
    emit sgnBeforeChanged(filepath, data);
}

void Diffs::onDiffReaded(QString filepath, QString data)
{
    clear();

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

        append(oper); // todo: удалить эту команду (она приватная)
    }

    emit sgnDiffChanged();
}
