#include "git_files_model.h"
#include <QDebug>

GitFilesModel::GitFilesModel() {}

void GitFilesModel::setFiles(QVector<GitFile> files, bool isStaged)
{
    beginResetModel();
    files_.clear();

    for (const auto &file : files) {
        if (isStaged) {
            if (file.indexState >= FileState::MODIFIED)
                files_ << file;
        } else {
            if (file.workState >= FileState::UNTRACKED)
                files_ << file;
        }
    }
    endResetModel();
}

int GitFilesModel::rowCount(const QModelIndex &parent) const {
  return files_.size();
}

int GitFilesModel::columnCount(const QModelIndex &parent) const { return 3; }

QVariant GitFilesModel::data(const QModelIndex &index, int role) const {
  if (index.isValid() == false)
    return QVariant();

  if (role == Qt::DisplayRole) {
    int pos = index.row();
    if (pos >= files_.size())
      return "err";

    switch (index.column()) {
    case 0:
      return files_[pos].name;
    case 1:
        return QString("%1 %2")
            .arg(stateToChar(files_[pos].indexState))
            .arg(stateToChar(files_[pos].workState));
    case 2:
      return files_[pos].path;
    }

    return index.column();
  }
  return {};
}

QVariant GitFilesModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const {
  if (orientation == Qt::Orientation::Vertical)
    return {};
  if (role == Qt::DisplayRole) {
    switch (section) {
    case 0:
      return "Filename";
    case 1:
      return "State";
    case 2:
      return "Path";
    }
  }

  return {};
}
