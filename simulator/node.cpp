#include "node.h"

/* Constructor: Node(int, int, int, QWidget *)
 * -------------------------------------------
 * Create node of radius 5 pixels
 * and associate given id number.
 * TODO: make size less hardcoded.
 */
Node::Node(int id, int elemId, QGraphicsItem *parent) :
    SchematicItem(id, "node", parent)
{
    setAcceptHoverEvents(true);
    rad = 5;
    this->elemId = elemId;
    line = QColor(Qt::black);
    fill = QColor(Qt::gray);
    hideNode();
}

// ----------- PUBLIC FUNCTIONS ----------------

/* Public Function: boundingRect()
 * -------------------------------
 * Returns rect containing node
 */
QRectF Node::boundingRect() const
{
    return QRectF(-rad, -rad, rad*2, rad*2);
}

/* Public Function: paint(QPainter *, ...)
 * ---------------------------------------
 * Draws node.
 */
void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(line);
    painter->setBrush(fill);
    painter->drawEllipse(QPoint(0,0), rad, rad);
    setZValue(1);
}

/* Public Function: hideNode()
 * ----------------------------
 * Set node invisible
 */
void Node::hideNode()
{
    line.setAlpha(0);
    fill.setAlpha(0);
    update();
}

/* Public Function: hideNode()
 * ----------------------------
 * Set node visible
 */
void Node::showNode()
{
    line.setAlpha(255);
    fill.setAlpha(255);
    update();
}

// ----------- EVENT HANDLERS ------------------

/* Event: hoverEnterEvent(QEvent *)
 * ---------------------------
 * Show node on hover
 *
 */
void Node::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if (elemId == -1)
        showNode();
    QGraphicsItem::hoverEnterEvent(event);
}

/* Event: leaveEvent(QEvent *)
 * ---------------------------
 * Hide node after hover
 */
void Node::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if (elemId == -1)
        hideNode();
    QGraphicsItem::hoverLeaveEvent(event);
}
