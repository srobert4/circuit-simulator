#include "node.h"

Node::Node(int x, int y, QWidget *parent) : QWidget(parent)
{
    setMouseTracking(true);
    setGeometry(x, y, 20, 20);
}

void Node::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.drawEllipse(0, 0, 20, 20);
}

//void Node::mouseMoveEvent(QMouseEvent *event)
//{
//    update();
//}

void Node::enterEvent(QEvent *)
{
    update();
    qInfo() << "enter event";
}

void Node::leaveEvent(QEvent *)
{
    hide();
    qInfo() << "exit event";
}
