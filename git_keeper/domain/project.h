#pragma once

#include "diff.h"

#include <QDir>
#include <QObject>

class Project : public QObject
{
    Q_OBJECT
public:
    Project(QString gitPath, QObject *parent = nullptr);
    void setGitPath(QString path);
    void setPath(QDir path);

    void clearDiffs();
    void addDiff(DiffOperation *oper);

    int getMappedLeftPos(int pos) const;
    int getMappedRightPos(int pos) const;

    const QVector<DiffOperation *> getCurrentFileDiffs() const;
    DiffOperation *getCurrentBlock();

    void setBlockForFile(const QString &filePath);
    void setCurrentBlock(int id);
    void movePrevChange();
    void moveNextChange();

    void clearCurrentBlockIndex();
    void selectCurrentFile(QString filepath);

    void status();
    void commit(QString message, bool isAmned);
    void queryLastCommitMessage();

signals:
    void sgnFilesChanged(QVector<GitFile> files);
    void sgnSendedToGit(QString data);
    void sgnReceivedFromGit(QString data, bool isError);
    void sgnLastMessageProcessed(QString data);
    void sgnCurrentContentReloaded(QString filepath, QString before, QString after);
    void sgnDiffsReloaded();

    void sgnCurrentFileChanged(QString filepath);
    void sgnCurrentBlockChanged(QString filepath);
    void sgnHasNextBlockChanged(bool hasNext);
    void sgnHasPrevBlockChanged(bool hasPrev);

private slots:
    void onGitStatusFinished(QVector<GitFile> result);
    void onSendedToGit(QString data);
    void onReceivedFromGit(QString data, bool isError);
    void onReceivedLastMessage(QString data);
    void onCurrentFileReaded(QString filepath, QString before, QString after);
    void onDiffsReaded(QStringList data);

private:
    QString currentFile_;
    QString gitPath_;
    GitRepository *gitRepository_ = nullptr;

    QVector<GitFile> changedFiles_;                       // result of "status" command
    QHash<QString, QVector<DiffOperation *>> operations_; // result of "diff" command
    QList<DiffOperation *> operationsList_;
    int currentOperationIndex_ = -1;
};
