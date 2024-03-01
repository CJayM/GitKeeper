#include "diff_mediator.h"

#include <QDebug>
#include <QPaintEvent>
#include <QPainter>

DiffMediator::DiffMediator(QWidget *parent) : QWidget(parent) {}

void DiffMediator::setBeforeBlocks(QVector<std::tuple<const DiffPos, QRect, QBrush>> blocks)
{
    beforeBlocks_ = blocks;
}

void DiffMediator::setAfterBlocks(QVector<std::tuple<const DiffPos, QRect, QBrush>> blocks)
{
    afterBlocks_ = blocks;
}

void DiffMediator::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.fillRect(event->rect(), Qt::white);

    int y1_1;
    int y1_2;
    int y2_1;
    int y2_2;

    int fullwidth = rect().width();
    int firstPoint = fullwidth * 0.25;
    int secondPoint = fullwidth * 0.75;
    QRect rect;
    QRect rect2;

    DiffPos diff1;
    DiffPos diff2;

    QVector<int> pairedBlocks;
    QBrush brush1;
    QBrush brush2;
    QLinearGradient grad(0, 0, fullwidth, 0);
    painter.setBrush(grad);
    painter.setPen(Qt::PenStyle::NoPen);

    for (const auto &block : qAsConst(beforeBlocks_)) {
        diff1 = std::get<0>(block);
        rect = std::get<1>(block);
        y1_1 = rect.top();
        y1_2 = rect.bottom();
        y2_1 = y1_1;
        y2_2 = y1_2;

        brush1 = std::get<2>(block);
        brush2 = brush1;

        bool finded = false;
        for (const auto &block2 : qAsConst(afterBlocks_)) {
            diff2 = std::get<0>(block2);
            if (diff2.id == diff1.id) {
                rect2 = std::get<1>(block2);
                y2_1 = rect2.top();
                y2_2 = rect2.bottom();
                brush2 = std::get<2>(block2);

                pairedBlocks << diff2.id;
                finded = true;
                break;
            }
        }

        QPainterPath path;
        path.moveTo(0, y1_1);
        path.cubicTo(firstPoint, y1_1, secondPoint, y2_1, fullwidth, y2_1);
        path.lineTo(fullwidth, y2_2);
        path.cubicTo(secondPoint, y2_2, firstPoint, y1_2, 0, y1_2);

        grad.setColorAt(0.35, brush1.color());
        grad.setColorAt(0.65, brush2.color());
        painter.setBrush(grad);

        painter.drawPath(path);
    }
}
