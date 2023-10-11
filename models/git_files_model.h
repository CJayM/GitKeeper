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

private:
  QVector<GitFile> files_;

// QAbstractItemModel interface
public:
QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};
