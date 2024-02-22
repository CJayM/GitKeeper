#include "app_palette.h"

QBrush AppPalette::DiffAddedLineBrush = QBrush(QColor(Qt::green).lighter(186));
QBrush AppPalette::DiffRemovedLineBrush = QBrush(QColor(Qt::red).lighter(186));
QBrush AppPalette::DiffUpdatedLineBrush = QBrush(QColor(Qt::blue).lighter(190));
QBrush AppPalette::DiffNothinkLineBrush = QBrush(QColor(Qt::gray).lighter(186));

QBrush AppPalette::DiffAddedLineExtraSelectionBrush = QBrush(QColor(Qt::green).lighter(160));
QBrush AppPalette::DiffRemovedLineExtraSelectionBrush = QBrush(QColor(Qt::red).lighter(160));
QBrush AppPalette::DiffUpdatedLineExtraSelectionBrush = QBrush(QColor(Qt::blue).lighter(170));
QBrush AppPalette::DiffNothinkLineExtraSelectionBrush = QBrush(QColor(Qt::gray).lighter(160));

QBrush AppPalette::DiffAddedLineDarkBrush = QBrush(QColor(Qt::green).lighter(80));
QBrush AppPalette::DiffRemovedLineDarkBrush = QBrush(QColor(Qt::red).lighter(80));
QBrush AppPalette::DiffUpdatedLineDarkBrush = QBrush(QColor(Qt::blue).lighter(80));
QBrush AppPalette::DiffNothinkLineDarkBrush = QBrush(QColor(Qt::gray).lighter(80));
