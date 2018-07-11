#include "node.h"

/* Constructor: Node(int, int, int, QWidget *)
 * -------------------------------------------
 * Create node centered at (x,y) of radius 5 pixels
 * and associate given id number.
 * TODO: make size less hardcoded.
 */
Node::Node(int x, int y, int id, QWidget *parent) : QWidget(parent)
{
    setMouseTracking(true);
    setGeometry(x - 20, y - 20, 40, 40);
    center = QPoint(20, 20);
    globalCenter = QPoint(x, y);
    rad = 5;
    this->id = id;
    show();
}

// ----------- EVENT HANDLERS ------------------

/* PaintEvent: paintEvent(QPaintEvent *)
 * -------------------------------------
 * If the most recent event is an enter or
 * drag, draw the node.
 */
void Node::paintEvent(QPaintEvent *)
{
    if (last_event == QEvent::Enter ||
            last_event == QEvent::DragEnter) {
        QPainter painter(this);
        painter.setPen(Qt::black);
        painter.drawEllipse(center, rad, rad);
    }
}

/* Event: enterEvent(QEvent *)
 * ---------------------------
 * Update most recent event type and
 * trigger paint event.
 */
void Node::enterEvent(QEvent *event)
{
    last_event = event->type();
    update();
}

/* Event: leaveEvent(QEvent *)
 * ---------------------------
 * Update most recent event type
 * and trigger paint event.
 */
void Node::leaveEvent(QEvent *event)
{
    last_event = event->type();
    update();
}

/* MouseEvent: mousePressEvent(QMouseEvent*)
 * -----------------------------------------
 * Emit nodeClicked() signal with center point
 * of node relative to Schematic and node id to
 * be caught by Schematic to stop or start drawing.
 */
void Node::mousePressEvent(QMouseEvent *)
{
    emit nodeClicked(globalCenter, id);
}
