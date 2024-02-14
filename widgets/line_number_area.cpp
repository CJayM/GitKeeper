#include "line_number_area.h"
#include "code_editor.h"

LineNumberArea::LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor_(editor) {}

QSize LineNumberArea::sizeHint() const
{
    return QSize(codeEditor_->lineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent *event)
{
    codeEditor_->lineNumberAreaPaintEvent(event);
}
