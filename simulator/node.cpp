#include "node.h"

/* Constructor: Node(int, int, int, QWidget *)
 * -------------------------------------------
 * Create node of radius 5 pixels
 * and associate given id number.
 * TODO: make size less hardcoded.
 */
Node::Node(int id, QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
    setAcceptHoverEvents(true);
    rad = 5;
    this->id = id;
//    hide();
}

// ----------- PUBLIC FUNCTIONS ----------------

/* Public Function: boundingRect()
 * -------------------------------
 * Returns rect containing node
 */
QRectF Node::boundingRect() const
{
    return QRectF(-5, -5, 10, 10);
}

/* Public Function: paint(QPainter *, ...)
 * ---------------------------------------
 * Draws node.
 */
void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(Qt::black);
    painter->setBrush(Qt::gray);
    painter->drawEllipse(QPoint(0,0), rad, rad);
}

// ----------- EVENT HANDLERS ------------------

/* Event: hoverEnterEvent(QEvent *)
 * ---------------------------
 * Update most recent event type and
 *
 */
void Node::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
    qInfo() << "Hovering over a node!";
    show();
}

/* Event: leaveEvent(QEvent *)
 * ---------------------------
 * Update most recent event type
 * and trigger paint event.
 */
void Node::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
    qInfo() << "Leaving node";
    hide();
}

/* MouseEvent: mousePressEvent(QMouseEvent*)
 * -----------------------------------------
 * Emit nodeClicked() signal with center point
 * of node relative to Schematic and node id to
 * be caught by Schematic to stop or start drawing.
 */
void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qInfo() << "clicked on node";
    emit nodeClicked(event->pos(), id);
}
