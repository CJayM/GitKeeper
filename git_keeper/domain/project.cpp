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
            &GitRepository::sgnCurrentFileReaded,
            this,
            &Project::onCurrentFileReaded);
    connect(gitRepository_,
            &GitRepository::sgnOriginalFileReaded,
            this,
            &Project::onOriginalFileReaded);
    connect(gitRepository_, &GitRepository::sgnDiffReaded, this, &Project::onDiffReaded);
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

void Project::clear()
{
    operations.clear();
    currentOperationIndex = -1;
}

void Project::append(DiffOperation oper)
{
    operations.append(oper);
}

int Project::getMappedLeftPos(int pos) const
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

int Project::getMappedRightPos(int pos) const
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

DiffOperation Project::getPrevChange()
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

DiffOperation Project::getNextChange()
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

void Project::selectCurrentFile(QString filepath)
{
    currentFile_ = filepath;
    qDebug() << "Current file:" << filepath;
    gitRepository_->queryFile(filepath);
    emit sgnCurrentFileChanged(filepath);
}

bool Project::hasNextFile() const
{
    if (changedFiles_.isEmpty())
        return false;

    bool finded = false;
    for (const auto &file : changedFiles_) {
        if (finded) {
            return true;
        }
        if (file.fullPath == currentFile_) {
            finded = true;
            continue;
        }
    }

    return false;
}

bool Project::hasPrevFile() const
{
    if (changedFiles_.isEmpty())
        return false;

    QString prevFile;
    for (const auto &file : changedFiles_) {
        if (file.fullPath == currentFile_) {
            if (prevFile.isEmpty() == false) {
                return true;
            }
            break;
        }
        prevFile = file.fullPath;
    }

    return false;
}

bool Project::selectNextFile()
{
    if (changedFiles_.isEmpty())
        return false;

    if (currentFile_.isEmpty() == true) {
        selectCurrentFile(changedFiles_.first().fullPath);
        return true;
    }

    bool finded = false;

    for (const auto &file : changedFiles_) {
        if (finded) {
            selectCurrentFile(file.fullPath);
            return true;
        }
        if (file.fullPath == currentFile_) {
            finded = true;
            continue;
        }
    }

    return false;
}

bool Project::selectPrevFile()
{
    QString prevFile;
    for (const auto &file : changedFiles_) {
        if (file.fullPath == currentFile_) {
            if (prevFile.isEmpty() == false) {
                selectCurrentFile(prevFile);
                return true;
            }
            break;
        }
        prevFile = file.fullPath;
    }

    return false;
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

void Project::onCurrentFileReaded(QString filepath, QString data)
{
    emit sgnAfterChanged(filepath, data);
}

void Project::onOriginalFileReaded(QString filepath, QString data)
{
    emit sgnBeforeChanged(filepath, data);
}

void Project::onDiffReaded(QString filepath, QString data)
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
