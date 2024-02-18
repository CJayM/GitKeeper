#include "diff.h"

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

void Diffs::clear()
{
    operations.clear();
    currentOperationIndex = -1;
}

void Diffs::append(DiffOperation oper)
{
    operations.append(oper);
}

int Diffs::getMappedLeftPos(int pos) const
{
    if (operations.isEmpty())
        return pos;

    const DiffOperation *minOper = &operations.first();
    const DiffOperation *maxOper = nullptr;

    for (const auto &oper : operations) {
        maxOper = &oper;
        if (maxOper->right.line >= pos)
            break;
        minOper = maxOper;
    }

    if (minOper == maxOper)
        return pos;

    float delta = maxOper->right.line - minOper->right.line;
    float percent = (pos - minOper->right.line) / delta;

    float delta2 = maxOper->left.line - minOper->left.line;
    float result = minOper->left.line + int((delta2 * percent));

    return result;
}

int Diffs::getMappedRightPos(int pos) const
{
    if (operations.isEmpty())
        return pos;

    const DiffOperation *minOper = &operations.first();
    const DiffOperation *maxOper = nullptr;

    for (const auto &oper : operations) {
        maxOper = &oper;
        if (maxOper->left.line >= pos)
            break;
        minOper = maxOper;
    }

    if (minOper == maxOper)
        return pos;

    float delta = maxOper->left.line - minOper->left.line;
    float percent = (pos - minOper->left.line) / delta;

    float delta2 = maxOper->right.line - minOper->right.line;
    float result = minOper->right.line + int((delta2 * percent));

    return result;
}

DiffOperation Diffs::getPrevChange()
{
    currentOperationIndex -= 1;
    if (currentOperationIndex < 0) {
        currentOperationIndex = 0;
        return {};
    }

    return operations[currentOperationIndex];
}

DiffOperation Diffs::getNextChange()
{
    currentOperationIndex += 1;
    if (currentOperationIndex >= operations.size()) {
        currentOperationIndex = operations.size() - 1;
        return {};
    }

    return operations[currentOperationIndex];
}
