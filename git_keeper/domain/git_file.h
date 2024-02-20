#pragma once

#include <QDir>
#include <QString>
#include <QStringList>

enum class FileState : qint8 {
    UNRECOGNIZED = -1,
    UNMODIFIED,
    IGNORED,
    UNTRACKED,
    MODIFIED,
    TYPE_CHANGED,
    ADDED,
    DELETED,
    RENAMED,
    COPIED,
    UPDATED_NOT_MERGED,
};

QChar stateToChar(FileState state);
FileState charToState(QChar ch);

struct GitFile {
  QString name;
  QDir path;
  FileState indexState;
  FileState workState;
  //  std::array<char, 4> submoduleState;
  QString submoduleState;
  //  std::array<char, 6> fileModeHead;
  QString fileModeHead;
  QString fileModeIndex;
  QString fileModeWorkTree;
  QString nameHead;
  QString nameIndex;

  QString fullPath() const;
};

struct CommandResult
{
    QStringList arguments;
    QStringList result;
};
