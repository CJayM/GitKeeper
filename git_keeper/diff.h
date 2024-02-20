#pragma once

#include <QDir>
#include <QObject>
#include <QVector>

#include "domain/git_file.h"

enum class DiffOperationType { UNINITIALIZED = -1, UNKNOWN = 0, REMOVE, ADD, REPLACE, NOTHINK };

struct DiffPos
{
    int line = -1;
    int count = -1;
    DiffOperationType type = DiffOperationType::UNINITIALIZED;
};

struct DiffOperation
{
    DiffPos left;
    DiffPos right;
};

class GitRepository;

class Diffs : public QObject
{
    Q_OBJECT
public:
    Diffs(QString gitPath, QObject *parent = nullptr);
    void setGitPath(QString path);
    void setPath(QDir path);

    QVector<DiffOperation> operations;
    void clear();
    void append(DiffOperation oper);
    int getMappedLeftPos(int pos) const;
    int getMappedRightPos(int pos) const;

    DiffOperation getPrevChange();
    DiffOperation getNextChange();
    void selectCurrentFile(QString filepath);
    bool hasNextFile() const;
    bool hasPrevFile() const;
    bool selectNextFile();
    bool selectPrevFile();

    void status();
    void commit(QString message, bool isAmned);
    void queryLastCommitMessage();

signals:
    void sgnFilesChanged(QVector<GitFile> files);
    void sgnSendedToGit(QString data);
    void sgnReceivedFromGit(QString data, bool isError);
    void sgnLastMessageProcessed(QString data);
    void sgnAfterChanged(QString filepath, QString data);
    void sgnBeforeChanged(QString filepath, QString data);
    void sgnDiffChanged();
    void sgnCurrentFileChanged(QString filepath);

private slots:
    void onGitStatusFinished(QVector<GitFile> result);
    void onSendedToGit(QString data);
    void onReceivedFromGit(QString data, bool isError);
    void onReceivedLastMessage(QString data);
    void onCurrentFileReaded(QString filepath, QString data);
    void onOriginalFileReaded(QString filepath, QString data);
    void onDiffReaded(QString filepath, QString data);

private:
    int currentOperationIndex = -1;
    QString currentFile_;
    QString gitPath_;
    GitRepository *gitRepository_ = nullptr;

    QVector<GitFile> changedFiles_;
};

void recognizeOperationType(DiffOperation &oper);
