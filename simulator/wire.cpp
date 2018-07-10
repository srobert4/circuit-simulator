#include <QtWidgets>
#include "wire.h"

Wire::Wire(QPoint start, QPoint end, QWidget *parent) : QWidget(parent)
{
    int x = (start.x() < end.x() ? start.x() : end.x());
    int y = (start.y() < end.y() ? start.y() : end.y());
    int width = abs(start.x() - end.x());
    int height = abs(start.y() - end.y());
    setGeometry(x, y, width, height);

    int start_x, start_y, end_x, end_y;
    if (start.x() < end.x()) {
        start_x = 0;
        end_x = width;
    } else {
        start_x = width;
        end_x = 0;
    }

    if (start.y() < end.y()) {
        start_y = 0;
        end_y = height;
    } else {
        start_y = height;
        end_y = 0;
    }

    this->start = QPoint(start_x, start_y);
    this->end = QPoint(end_x, end_y);
    update();
    show();
}

void Wire::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.drawLine(start, end);
}
