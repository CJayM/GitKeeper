#include <QAbstractTableModel>

#include <domain/git_file.h>

#pragma once

class GitFilesModel : public QAbstractTableModel {
public:
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
