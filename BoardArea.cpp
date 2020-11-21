#include "BoardArea.h"

BoardArea::BoardArea(QWidget *parent) : QWidget(parent)
{
    blockSize = 24;
    pBoard = nullptr;
    isModify = false;
    BindBoard(TetrisAI::PBoard);
    setAutoFillBackground(true);
    UpdateBoard();
}

QSize BoardArea::minimumSizeHint() const
{
    return QSize(250, 300);
}

QSize BoardArea::sizeHint() const
{
    return QSize(250, 300);
}

void BoardArea::UpdateBoard()
{
    update();
}

void BoardArea::BindBoard(BoardState * pBoard)
{
    this->pBoard = pBoard;
}

void BoardArea::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(0, 0, 0), 1));
    painter.drawRect(0, 0, 250, 298);
    DrawBoard(painter);

    if (pBoard != nullptr)
    {
        for (int x = 0; x < COL; x++)
        {
            for (int y = 0; y < ROW; y++)
            {
                DrawBlock(painter, x, y);
            }
        }
    }
}

void BoardArea::mouseReleaseEvent(QMouseEvent * event)
{
    if (isModify && event->button() == Qt::LeftButton)
    {
        int x = (event->x() - 5) / blockSize;
        int y = (event->y() - 5) / blockSize;
        if (pBoard && x >= 0 && x < COL && y >= 0 && y < ROW)
            TetrisAI::ReverseBlock(x, y);
        update();
    }
}

void BoardArea::DrawBlock(QPainter & painter, const int x, const int y)
{
    QColor color(255, 255, 255);
    if ((*pBoard)[y][x])
        color = QColor(200, 200, 200);
    painter.fillRect(7 + x * blockSize, 7 + y * blockSize,
        blockSize - 4, blockSize - 4, color);
}

void BoardArea::DrawBlock(QPainter & painter, const int x, const int y, const QColor & color)
{
    painter.fillRect(7 + x * blockSize, 7 + y * blockSize,
        blockSize - 4, blockSize - 4, color);
}

void BoardArea::DrawBoard(QPainter & painter)
{
    painter.setPen(QPen(QColor(200, 200, 200), 2));
    for (int x = 0; x <= COL; x++)
        painter.drawLine(5 + x * blockSize, 5, 5 + x * blockSize, 5 + blockSize * ROW);
    for (int y = 0; y <= ROW; y++)
        painter.drawLine(5, 5 + y * blockSize, 5 + blockSize * COL, 5 + y * blockSize);
}

