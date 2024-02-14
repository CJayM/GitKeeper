#include "code_editor.h"
#include "line_number_area.h"

#include <QDebug>
#include <QFont>
#include <QPaintEvent>
#include <QPainter>
#include <QTextBlock>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    QFont font("Arial", 13);
    setFont(font);

    QFontMetrics metrics(font);

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
}

void CodeEditor::addDiffBlock(DiffPos block)
{
    diffBlocks_ << block;
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
    auto doc = document();

    auto offset = contentOffset().toPoint();
    for (const auto &diff : qAsConst(diffBlocks_)) {
        auto block = doc->findBlockByLineNumber(diff.line - 1);
        auto geo = blockBoundingGeometry(block).toRect();
        if (diff.count > 1) {
            for (int i = 0; i < diff.count; ++i) {
                auto blockEnd = doc->findBlockByLineNumber(diff.line - 1 + i);
                auto geoEnd = blockBoundingGeometry(blockEnd).toRect();
                if (rect.intersects(geoEnd))
                    geo.setBottom(geoEnd.bottom());
            }
        }
        if (diff.count == 0) {
            if (diff.type == DiffOperationType::NOTHINK) {
                auto newRect = geo;
                newRect.setHeight(4);
                painter.fillRect(newRect, QColor(Qt::red).lighter(160));
            }
            if (diff.type == DiffOperationType::REMOVE) {
                auto newRect = geo;
                newRect.setHeight(4);
                painter.fillRect(newRect, QColor(Qt::cyan).lighter(160));
            }
            if (diff.type == DiffOperationType::REPLACE) {
                auto newRect = geo;
                newRect.setHeight(4);
                painter.fillRect(newRect, QColor(Qt::darkBlue).lighter(160));
            }
            continue;
        }

        geo.translate(offset);
        if (rect.intersects(geo) == false)
            continue;

        if (diff.type == DiffOperationType::ADD)
            painter.fillRect(geo, QColor(Qt::green).lighter(160));
        if (diff.type == DiffOperationType::REMOVE)
            painter.fillRect(geo, QColor(Qt::red).lighter(160));
        if (diff.type == DiffOperationType::REPLACE)
            painter.fillRect(geo, QColor(Qt::blue).lighter(160));
        if (diff.type == DiffOperationType::NOTHINK)
            painter.fillRect(geo, QColor(Qt::gray).lighter(160));
    }

    QPlainTextEdit::paintEvent(e);

    //    QTextBlock block = firstVisibleBlock();
    //    int blockNumber = block.blockNumber();
    //    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    //    int bottom = top + qRound(blockBoundingRect(block).height());

    //    while (block.isValid() && top <= e->rect().bottom()) {
    //        if (block.isVisible() && bottom >= e->rect().top()) {
    //            QString number = QString::number(blockNumber + 1);
    //            painter.setPen(Qt::lightGray);
    //            painter.drawText(-2,
    //                             top,
    //                             lineNumberArea_->width(),
    //                             fontMetrics().height(),
    //                             Qt::AlignRight,
    //                             number);
    //        }

    //        block = block.next();
    //        top = bottom;
    //        bottom = top + qRound(blockBoundingRect(block).height());
    //        ++blockNumber;
    //    }
}
