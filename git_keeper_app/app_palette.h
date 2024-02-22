#pragma once

#include <QBrush>

class AppPalette
{
public:
    static QBrush DiffAddedLineBrush;
    static QBrush DiffRemovedLineBrush;
    static QBrush DiffUpdatedLineBrush;
    static QBrush DiffNothinkLineBrush;

    static QBrush DiffAddedLineExtraSelectionBrush;
    static QBrush DiffRemovedLineExtraSelectionBrush;
    static QBrush DiffUpdatedLineExtraSelectionBrush;
    static QBrush DiffNothinkLineExtraSelectionBrush;

    static QBrush DiffAddedLineDarkBrush;
    static QBrush DiffRemovedLineDarkBrush;
    static QBrush DiffUpdatedLineDarkBrush;
    static QBrush DiffNothinkLineDarkBrush;

    AppPalette(){};
};
