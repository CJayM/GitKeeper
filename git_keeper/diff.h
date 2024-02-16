#include <QVector>

#pragma once

enum class DiffOperationType { UNKNOWN = 0, REMOVE, ADD, REPLACE, NOTHINK };

struct DiffPos
{
    int line;
    int count;
    DiffOperationType type;
};

struct DiffOperation
{
    DiffPos left;
    DiffPos right;
};

class Diffs
{
public:
    QVector<DiffOperation> operations;
    void clear();
    void append(DiffOperation oper);
    int getMappedPos(int pos) const;
};

void recognizeOperationType(DiffOperation &oper);
