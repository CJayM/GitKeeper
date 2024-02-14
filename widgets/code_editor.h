#pragma once

#include "diff.h"

#include <QPlainTextEdit>
#include <QWidget>

class CodeEditor;

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    CodeEditor *codeEditor_ = nullptr;
};

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit CodeEditor(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth() const;

    void clearDiffBlocks();
    void addDiffBlock(DiffPos block);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *lineNumberArea_;
    QVector<DiffPos> diffBlocks_;
    QList<QTextEdit::ExtraSelection> diffSelections_;
};