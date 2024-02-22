#include "code_editor.h"
#include "line_number_area.h"

#include <app_palette.h>
#include <QDebug>
#include <QFont>
#include <QPaintEvent>
#include <QPainter>
#include <QTextBlock>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    QFont font("Arial", 13);
    setFont(font);

    lineNumberArea_ = new LineNumberArea(this);

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);

    updateLineNumberAreaWidth(0);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea_);
    painter.fillRect(event->rect(), Qt::white);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::lightGray);
            painter.drawText(-2,
                             top,
                             lineNumberArea_->width(),
                             fontMetrics().height(),
                             Qt::AlignRight,
                             number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

int CodeEditor::lineNumberAreaWidth() const
{
    int digits = 1;
    int max = std::max(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 8 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void CodeEditor::clearDiffBlocks()
{
    diffBlocks_.clear();
    currentDiffBlockIndex_ = -1;
}

void CodeEditor::addDiffBlock(DiffPos block)
{
    diffBlocks_ << block;
}

void CodeEditor::setCurrentBlock(const DiffPos &diff)
{
    currentDiffBlockIndex_ = diff.id;
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    lineNumberArea_->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}
void CodeEditor::updateLineNumberAreaWidth(int newBlockCount)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea_->scroll(0, dy);
    else
        lineNumberArea_->update(0, rect.y(), lineNumberArea_->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::paintEvent(QPaintEvent *e)
{
    QPainter painter(viewport());
    QRect rect = e->rect();
    painter.fillRect(rect, Qt::white);
    auto doc = document();

    auto offset = contentOffset().toPoint();

    auto lineBrush = AppPalette::DiffAddedLineBrush;
    auto borderBrush = AppPalette::DiffAddedLineDarkBrush;

    for (const auto &diff : qAsConst(diffBlocks_)) {
        auto block = doc->findBlockByLineNumber(diff.line - 1);
        block = doc->findBlockByNumber(block.firstLineNumber());
        auto geo = blockBoundingGeometry(block).toRect();
        geo.adjust(0, offset.y(), 0, offset.y());

        if (diff.count > 1) {
            for (int i = 0; i < diff.count; ++i) {
                auto blockEnd = doc->findBlockByLineNumber(diff.line - 1 + i);
                blockEnd = doc->findBlockByNumber(blockEnd.firstLineNumber());
                auto geoEnd = blockBoundingGeometry(blockEnd).toRect();                
                if (rect.intersects(geoEnd))
                    geo.setBottom(geoEnd.bottom());
            }
        }
        if (diff.count == 0) {
            auto newRect = geo;
            QPoint topLeft{0, newRect.bottom() - 2};
            newRect = QRect(topLeft, newRect.bottomRight());
            newRect.adjust(0, 2, 0, 2);

            if (diff.type == DiffOperationType::ADD)
                painter.fillRect(newRect, AppPalette::DiffAddedLineBrush);

            if (diff.type == DiffOperationType::NOTHINK)
                painter.fillRect(newRect, AppPalette::DiffAddedLineBrush);

            if (diff.type == DiffOperationType::REMOVE)
                painter.fillRect(newRect, AppPalette::DiffRemovedLineBrush);

            if (diff.type == DiffOperationType::REPLACE)
                painter.fillRect(newRect, AppPalette::DiffUpdatedLineBrush);
        }

        if (rect.intersects(geo) == false)
            continue;

        if (diff.type == DiffOperationType::ADD) {
            lineBrush = AppPalette::DiffAddedLineBrush;
            borderBrush = AppPalette::DiffAddedLineDarkBrush;
        }

        if (diff.type == DiffOperationType::REMOVE) {
            lineBrush = AppPalette::DiffRemovedLineBrush;
            borderBrush = AppPalette::DiffRemovedLineDarkBrush;
        }
        if (diff.type == DiffOperationType::REPLACE) {
            lineBrush = AppPalette::DiffUpdatedLineBrush;
            borderBrush = AppPalette::DiffUpdatedLineDarkBrush;
        }
        if (diff.type == DiffOperationType::NOTHINK) {
            lineBrush = AppPalette::DiffNothinkLineBrush;
            borderBrush = AppPalette::DiffNothinkLineDarkBrush;
        }

        if (diff.count != 0) {
            painter.fillRect(geo, lineBrush);
        }

        if (diff.id == currentDiffBlockIndex_) {
            if (diff.count == 0) {
                geo.setTop(geo.bottom() + 3);
                geo.adjust(0, 0, 0, -1);
            }
            geo.setRight(geo.left() + 4);
            painter.fillRect(geo, borderBrush);
        }
    }

    QPlainTextEdit::paintEvent(e);
}
