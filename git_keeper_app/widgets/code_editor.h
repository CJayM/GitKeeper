#pragma once

#include "diff.h"

#include <QPlainTextEdit>
#include <QWidget>

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit CodeEditor(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
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
    QWidget *lineNumberArea_;
    QVector<DiffPos> diffBlocks_;
    int currentDiffBlockIndex_ = -1;

    QVector<std::tuple<const DiffPos &, QRect, QBrush>> visibleBlocks_;

    void recalcVisibleBlockAreas(const QRect &rect);
};
