#pragma once

#include "diff.h"
#include <QWidget>

class DiffMediator : public QWidget
{
    Q_OBJECT
public:
    enum class Side { BEFORE, AFTER };
    explicit DiffMediator(QWidget *parent = nullptr);
    void setBeforeBlocks(QVector<std::tuple<const DiffPos, QRect, QBrush>> blocks);
    void setAfterBlocks(QVector<std::tuple<const DiffPos, QRect, QBrush>> blocks);

signals:

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<std::tuple<const DiffPos, QRect, QBrush>> beforeBlocks_;
    QVector<std::tuple<const DiffPos, QRect, QBrush>> afterBlocks_;
};
