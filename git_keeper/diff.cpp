#include "diff.h"

#include "domain/git_repository.h"

#include <QDebug>

void recognizeOperationType(DiffOperation &oper)
{
    if (oper.left.count > 0 && oper.right.count == 0) {
        oper.left.type = DiffOperationType::REMOVE;
        oper.right.type = DiffOperationType::REMOVE;
        return;
    }

    if (oper.left.count == 0 && oper.right.count > 0) {
        oper.left.type = DiffOperationType::ADD;
        oper.right.type = DiffOperationType::ADD;
        return;
    }

    oper.left.type = DiffOperationType::REPLACE;
    oper.right.type = DiffOperationType::ADD;
    return;
}
