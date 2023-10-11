#pragma once

#include <QString>

enum class FileState {
  UNRECOGNIZED,
  UNMODIFIED,
  MODIFIED,
  TYPE_CHANGED,
  ADDED,
  DELETED,
  RENAMED,
  COPIED,
  UPDATED_NOT_MERGED,
  UNTRACKED,
  IGNORED,
};

QChar stateToChar(FileState state);
FileState charToState(QChar ch);

struct GitFile {
  QString name;
  QString path;
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
};
