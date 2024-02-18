#include <QVector>

#pragma once

enum class DiffOperationType { UNINITIALIZED = -1, UNKNOWN = 0, REMOVE, ADD, REPLACE, NOTHINK };

struct DiffPos
{
    int line = -1;
    int count = -1;
    DiffOperationType type = DiffOperationType::UNINITIALIZED;
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
    int getMappedLeftPos(int pos) const;
    int getMappedRightPos(int pos) const;

    DiffOperation getPrevChange();
    DiffOperation getNextChange();

    int currentOperationIndex = -1;
};

void recognizeOperationType(DiffOperation &oper);
