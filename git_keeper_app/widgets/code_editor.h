#pragma once

#include "diff.h"
#include "diff_mediator.h"

#include <QPlainTextEdit>
#include <QWidget>

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
public:    
    explicit CodeEditor(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    void setDiffMediator(DiffMediator *mediator, DiffMediator::Side side); // 0-left, 1-right
    int lineNumberAreaWidth() const;

    void clearDiffBlocks();
    void addDiffBlock(DiffPos block);
    void setCurrentBlock(const DiffPos &diff);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *e) override;

signals:
    void sgnScrolledToBlock(int id);

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);
    void onVScrollBarChanged(int value);

private:
    QWidget *lineNumberArea_ = nullptr;
    DiffMediator *diffMediatorWidget_ = nullptr;
    QVector<DiffPos> diffBlocks_;
    int currentDiffBlockIndex_ = -1;
    DiffMediator::Side side_;

    QVector<std::tuple<const DiffPos, QRect, QBrush>> visibleBlocks_;

    void recalcVisibleBlockAreas(const QRect &rect);
};
