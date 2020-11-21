#ifndef BOARDAREA_H
#define BOARDAREA_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include "TetrisAI.h"

class BoardArea : public QWidget
{
    Q_OBJECT
public:
    explicit BoardArea(QWidget *parent = nullptr);

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    void UpdateBoard();
    void BindBoard(BoardState * pBoard);
    bool IsModifyBoard() { return isModify; }
    void ReverseModify() { isModify = !isModify; }

protected:
    void paintEvent(QPaintEvent * event) override;
    virtual void mouseReleaseEvent(QMouseEvent * event) override;

private:
    BoardState * pBoard;
    int blockSize;
    bool isModify;
    void DrawBlock(QPainter & painter, const int x, const int y);
    void DrawBlock(QPainter & painter, const int x, const int y, const QColor &color);
    void DrawBoard(QPainter & painter);
};

#endif // BOARDAREA_H
