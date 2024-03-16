#include "code_editor.h"

#include "line_number_area.h"

#include "app_palette.h"
#include <QDebug>
#include <QFont>
#include <QPaintEvent>
#include <QPainter>
#include <QScrollBar>
#include <QTextBlock>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    QFont font("Arial", 13);
    setFont(font);
    setCenterOnScroll(true);

    lineNumberArea_ = new LineNumberArea(this);

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);

    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &CodeEditor::onVScrollBarChanged);

    updateLineNumberAreaWidth(0);
    recalcVisibleBlockAreas();
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

void CodeEditor::setDiffMediator(DiffMediator *mediator, DiffMediator::Side side)
{
    diffMediatorWidget_ = mediator;
    side_ = side;
    viewport()->repaint();
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
    recalcVisibleBlockAreas();
}

void CodeEditor::addDiffBlock(DiffPos block)
{
    diffBlocks_ << block;
    recalcVisibleBlockAreas();
}

void CodeEditor::setCurrentBlockId(int id)
{
    currentDiffBlockIndex_ = id;
    recalcVisibleBlockAreas();
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    lineNumberArea_->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
    recalcVisibleBlockAreas();
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

void CodeEditor::onVScrollBarChanged(int value)
{
    static int OFFSET = 2; // количество строк
    int centered_line = value + verticalScrollBar()->pageStep() / 2;
    int minDistance = -1;
    int nearestId = -1;
    for (const auto diff : diffBlocks_) {
        if (diff.line < centered_line - OFFSET)
            continue;

        if (diff.line > centered_line + OFFSET)
            break;

        int dist = abs(diff.line - centered_line);
        if (minDistance == -1 || minDistance > dist) {
            minDistance = dist;
            nearestId = diff.id;
        }
    }
    setCurrentBlockId(nearestId);
    emit sgnCurrentBlockChanged(nearestId);
}

void CodeEditor::recalcVisibleBlockAreas()
{
    auto doc = document();
    auto offset = contentOffset().toPoint();
    visibleBlocks_.clear();

    auto newRect = this->rect();

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
                if (newRect.intersects(geoEnd))
                    geo.setBottom(geoEnd.bottom());
            }
        }
        if (diff.count == 0) {
            auto newRect = geo;
            QPoint topLeft{0, newRect.bottom() - 2};
            newRect = QRect(topLeft, newRect.bottomRight());
            newRect.adjust(0, 2, 0, 2);

            if (diff.type == DiffOperationType::ADD)
                visibleBlocks_.append(
                    std::make_tuple(diff, newRect, AppPalette::DiffAddedLineBrush));

            if (diff.type == DiffOperationType::NOTHINK)
                visibleBlocks_.append(
                    std::make_tuple(diff, newRect, AppPalette::DiffAddedLineBrush));

            if (diff.type == DiffOperationType::REMOVE)
                visibleBlocks_.append(
                    std::make_tuple(diff, newRect, AppPalette::DiffRemovedLineBrush));

            if (diff.type == DiffOperationType::REPLACE)
                visibleBlocks_.append(
                    std::make_tuple(diff, newRect, AppPalette::DiffUpdatedLineBrush));
        }

        if (newRect.intersects(geo) == false)
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
            visibleBlocks_.append(std::make_tuple(diff, geo, lineBrush));
        }
    }

    if (diffMediatorWidget_) {
        if (side_ == DiffMediator::Side::BEFORE)
            diffMediatorWidget_->setBeforeBlocks(visibleBlocks_);
        if (side_ == DiffMediator::Side::AFTER)
            diffMediatorWidget_->setAfterBlocks(visibleBlocks_);
    }

    viewport()->repaint();
    return;
}

void CodeEditor::mousePressEvent(QMouseEvent *event)
{
    auto doc = document();
    auto curs = cursorForPosition(event->pos());
    auto block = doc->findBlock(curs.position());

    int line = block.firstLineNumber() + 1;

    for (const auto &diff : qAsConst(diffBlocks_)) {
        int from = diff.line;
        int to = from + diff.count;
        if (to < from)
            continue;
        if (from > line)
            break;

        if ((line >= from) && (line <= to)) {
            if (currentDiffBlockIndex_ != diff.id) {
                setCurrentBlockId(diff.id);
                emit sgnCurrentBlockChanged(diff.id);
                return;
            }
        }
    }

    QPlainTextEdit::mousePressEvent(event);
}

void CodeEditor::paintEvent(QPaintEvent *e)
{    
    QPainter painter(viewport());
    QRect rect = e->rect();
    painter.fillRect(rect, Qt::white);

    if (diffMediatorWidget_)
        diffMediatorWidget_->update();

    for (const auto &block : qAsConst(visibleBlocks_)) {
        auto diff = std::get<0>(block);
        auto currentRect = std::get<1>(block);

        auto fillColor = std::get<2>(block);

        painter.fillRect(currentRect, fillColor);

        if (currentDiffBlockIndex_ != -1) {
            if (currentDiffBlockIndex_ == diff.id) {
                fillColor = fillColor.color().darker(120);
                painter.setPen(fillColor.color());

                painter.drawLine(currentRect.topLeft(), currentRect.topRight());
                painter.drawLine(currentRect.bottomLeft(), currentRect.bottomRight());
            }
        }
    }

    QPlainTextEdit::paintEvent(e);
}
