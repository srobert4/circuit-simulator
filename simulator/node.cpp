#include "node.h"

Node::Node(int x, int y, int id, QWidget *parent) : QWidget(parent)
{
    setMouseTracking(true);
//    setAttribute(Qt::WA_NoMousePropagation);
    setGeometry(x - 20, y - 20, 40, 40);
    center = QPoint(20, 20);
    globalCenter = QPoint(x, y);
    rad = 5;
    this->id = id;
    show();
}

void Node::paintEvent(QPaintEvent *)
{
    if (last_event == QEvent::Enter ||
            last_event == QEvent::DragEnter) {
        QPainter painter(this);
        painter.setPen(Qt::black);
        painter.drawEllipse(center, rad, rad);
    }
}

void Node::enterEvent(QEvent *event)
{
    last_event = event->type();
    update();
}

void Node::leaveEvent(QEvent *event)
{
    last_event = event->type();
    update();
}

void Node::mousePressEvent(QMouseEvent *)
{
    emit nodeClicked(globalCenter, id);
}
