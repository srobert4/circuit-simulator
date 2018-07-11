#include <QtWidgets>
#include "wire.h"

/* Constructor: Wire(QPoint, QPoint, int, QWidget *)
 * -------------------------------------------------
 * Constructs a wire from the start point to the end
 * point and associates it with an ID number.
 */
Wire::Wire(QPoint start, QPoint end, int id, QWidget *parent) : QWidget(parent)
{
    this->id = id;
    node1 = -1; // public setters set nodes
    node2 = -1;

    int x = (start.x() < end.x() ? start.x() : end.x());
    int y = (start.y() < end.y() ? start.y() : end.y());
    int width = abs(start.x() - end.x());
    int height = abs(start.y() - end.y());
    // set shape of QWidget
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

    // set start and end points of wire
    this->start = QPoint(start_x, start_y);
    this->end = QPoint(end_x, end_y);

    update(); // trigger paint event
    show();
}

// ----------- EVENT HANDLERS ---------------

/* PaintEvent: paintEvent(QPaintEvent*)
 * ------------------------------------
 * Paint wire from start and end instance vars
 */
void Wire::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.drawLine(start, end);
}
