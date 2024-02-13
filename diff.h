#pragma once

enum class DiffOperationType { UNKNOWN = 0, REMOVE, ADD, REPLACE };

struct DiffPos
{
    int line;
    int count;
};

struct DiffOepration
{
    DiffPos left;
    DiffPos right;
    DiffOperationType type;
};

void recognizeOperationType(DiffOepration &oper);
