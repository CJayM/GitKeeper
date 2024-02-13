#include "diff.h"

void recognizeOperationType(DiffOepration &oper)
{
    oper.type = DiffOperationType::UNKNOWN;

    if (oper.left.count > 0 && oper.right.count == 0) {
        oper.type = DiffOperationType::REMOVE;
        return;
    }

    if (oper.left.count > 0 && oper.right.count > 0) {
        oper.type = DiffOperationType::REPLACE;
        return;
    }

    if (oper.left.count == 0 && oper.right.count > 0) {
        oper.type = DiffOperationType::ADD;
        return;
    }
}
