#include "node.h"

/* Constructor: Node(int, int, int, QWidget *)
 * -------------------------------------------
 * Create node of radius 5 pixels
 * and associate given id number.
 * TODO: make size less hardcoded.
 */
Node::Node(SchematicItem *element, QGraphicsItem *parent) :
    SchematicItem(-1, "node", parent)
{
    setAcceptHoverEvents(true);
    setFlag(ItemSendsScenePositionChanges);
    if (!element) setFlag(ItemIsMovable);
    this->element = element;
    line = QColor(Qt::black);
    fill = QColor(Qt::gray);
    wire = QPen(Qt::black);
    wire.setWidth(2);
    if (element) hideNode();
}

Node::~Node()
{
    Connection thisC;
    thisC.node = this;
    for (Connection c : xNodes)
        c.node->removeYNode(thisC);
    for (Connection c : yNodes)
        c.node->removeXNode(thisC);
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
    // draw node
    painter->setPen(line);
    painter->setBrush(fill);
    painter->drawEllipse(QPointF(0,0), rad, rad);
    // draw wires
    for (Connection c : xNodes) {
        c.line->setLine(0, 0, mapFromScene(c.node->scenePos()).x(), 0);
    }

    for (Connection c : yNodes) {
        c.line->setLine(0, 0, 0, mapFromScene(c.node->scenePos()).y());
    }
}

void Node::connectNode(Node *node)
{
    Connection cnx;
    cnx.node = node;
    cnx.line = new QGraphicsLineItem(0, 0, 0, 0, this);
    cnx.line->setPen(wire);

    Connection thisCnx;
    thisCnx.node = this;
    thisCnx.line = new QGraphicsLineItem(0, 0, 0, 0, node);
    thisCnx.line->setPen(wire);

    qreal midpt = scene()->width() / 2;
    if (this->x() > midpt) {
        if (this->x() > node->x()) {
            this->addYNode(cnx);
            node->addXNode(thisCnx);
        } else{
            this->addXNode(cnx);
            node->addYNode(thisCnx);
        }
    } else {
        if (this->x() > node->x()) {
            this->addXNode(cnx);
            node->addYNode(thisCnx);
        } else {
            this->addYNode(cnx);
            node->addXNode(thisCnx);
        }
    }
}

void Node::removeXNode(Connection c)
{
    Connection cnx = xNodes[xNodes.indexOf(c)];
    cnx.line->setLine(0, 0, 0, 0);
    xNodes.removeOne(cnx);
}

void Node::removeYNode(Connection c)
{
    Connection cnx = yNodes[yNodes.indexOf(c)];
    cnx.line->setLine(0, 0, 0, 0);
    yNodes.removeOne(cnx);
}

void Node::connectToElement(SchematicItem *element)
{
    this->element = element;
    setFlag(ItemIsMovable, false);
    hideNode();
}


/* Public Function: hideNode()
 * ----------------------------
 * Set node invisible
 */
void Node::hideNode()
{
    return;
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
    showNode();
    QGraphicsItem::hoverEnterEvent(event);
}

/* Event: leaveEvent(QEvent *)
 * ---------------------------
 * Hide node after hover
 */
void Node::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    hideNode();
    QGraphicsItem::hoverLeaveEvent(event);
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemScenePositionHasChanged) {
        for (Connection c : xNodes + yNodes)
            c.node->update();
    }
    update();
    return QGraphicsItem::itemChange(change, value);
}
