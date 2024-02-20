#include "git_file.h"

QChar stateToChar(FileState state) {
  switch (state) {
  case FileState::UNMODIFIED:
    return '.';
  case FileState::MODIFIED:
    return 'M';
  case FileState::TYPE_CHANGED:
    return 'T';
  case FileState::ADDED:
    return 'A';
  case FileState::DELETED:
    return 'D';

  case FileState::RENAMED:
    return 'R';
  case FileState::COPIED:
    return 'C';

  case FileState::UPDATED_NOT_MERGED:
    return 'U';
  case FileState::UNTRACKED:
    return '?';
  case FileState::IGNORED:
    return '!';
  }
  return '-';
}

FileState charToState(QChar ch) {
  if (ch == ' ' or ch == '.')
    return FileState::UNMODIFIED;
  if (ch == 'M')
    return FileState::MODIFIED;
  if (ch == 'T')
    return FileState::TYPE_CHANGED;
  if (ch == 'A')
    return FileState::ADDED;
  if (ch == 'D')
    return FileState::DELETED;
  if (ch == 'R')
    return FileState::RENAMED;
  if (ch == 'C')
    return FileState::COPIED;
  if (ch == 'U')
    return FileState::UPDATED_NOT_MERGED;
  if (ch == '?')
    return FileState::UNTRACKED;
  if (ch == '!')
    return FileState::IGNORED;

  return FileState::UNRECOGNIZED;
}

QString GitFile::fullPath() const
{
    return path.filePath(name);
}

void git_file_register_metatypes()
{
    qRegisterMetaType<QVector<GitFile>>("QVector<GitFile>");
}
