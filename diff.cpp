#include "diff.h"

void recognizeOperationType(DiffOperation &oper)
{
    if (oper.left.count > 0 && oper.right.count == 0) {
        oper.left.type = DiffOperationType::REMOVE;
        oper.right.type = DiffOperationType::NOTHINK;
        return;
    }

    if (oper.left.count == 0 && oper.right.count > 0) {
        oper.left.type = DiffOperationType::NOTHINK;
        oper.right.type = DiffOperationType::ADD;
        return;
    }

    oper.left.type = DiffOperationType::REPLACE;
    oper.right.type = DiffOperationType::ADD;
    return;
}

void Diffs::clear()
{
    operations.clear();
}

void Diffs::append(DiffOperation oper)
{
    operations.append(oper);
}
