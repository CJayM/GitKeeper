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
    QString filePath;
    DiffPos left;
    DiffPos right;
};

class GitRepository;

void recognizeOperationType(DiffOperation *oper);
