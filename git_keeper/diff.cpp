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

int Diffs::getMappedPos(int pos) const
{
    return pos;
    int leftMin = 0;
    int leftMax = pos;
    int rightMin = 0;
    int rightMax = pos;

    for (const auto &oper : operations) {
        if (pos > oper.right.line) {
            rightMin = oper.right.line;
            leftMin = oper.left.line;
        }

        if (pos < oper.right.line) {
            rightMax = oper.right.line + oper.right.count;
            leftMax = oper.left.line + oper.left.count;
            break;
        }
    }

    float percent = 1.0;
    if (rightMax != rightMin)
        percent = (pos - rightMin) / (rightMax - rightMin);
    float result = (leftMax - leftMin) * percent + leftMin;

    return int(result);
}
