#pragma once

#include "domain/git_file.h"
#include <QAbstractTableModel>

class GitFilesModel : public QAbstractTableModel
{
public:
    const static int NAME_ROLE = Qt::UserRole + 1;
    const static int PATH_ROLE = Qt::UserRole + 2;
    const static int FULL_PATH_ROLE = Qt::UserRole + 3;

    GitFilesModel();

    void setFiles(QVector<GitFile> files);

    // QAbstractItemModel interface
public:
  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;

  void setIsStaged(bool isStaged);

  private:
  QVector<GitFile> files_;
  bool isStaged_ = false;

  // QAbstractItemModel interface
  public:
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};
