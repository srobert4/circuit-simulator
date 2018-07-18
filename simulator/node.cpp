#include "node.h"

/* Constructor: Node(SchematicItem*, QGraphicsItem*)
 * -------------------------------------------
 * Create node and set flags. Make moveable and visible
 * if not associated with an element.
 */
Node::Node(SchematicItem *element, QGraphicsItem *parent) :
    SchematicItem("node", parent)
{
    setAcceptHoverEvents(true);
    setFlag(ItemIsSelectable);
    setFlag(ItemIsFocusable);
    setFlag(ItemSendsScenePositionChanges);
    if (!element) setFlag(ItemIsMovable);

    this->element = element;
    line = QColor(Qt::black);
    fill = QColor(Qt::gray);
    wire = QPen(Qt::black);
    wire.setWidth(2);
    if (element) hideNode();
}

/* Destructor: ~Node()
 * -------------------
 * Remove the node from all connected
 * nodes tracking list so that no wires
 * are drawn to it anymore.
 */
Node::~Node()
{
    Connection thisC;
    thisC.node = this;
    for (Connection c : xNodes)
        c.node->removeYNode(thisC);
    for (Connection c : yNodes)
        c.node->removeXNode(thisC);

    if(label) delete label;
}

// ========= PUBLIC FUNCTIONS ============================

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
        c.line->setPen(wire);
        c.line->setLine(0, 0, mapFromScene(c.node->scenePos()).x(), 0);
    }

    for (Connection c : yNodes) {
        c.line->setPen(wire);
        c.line->setLine(0, 0, 0, mapFromScene(c.node->scenePos()).y());
    }
}

/* Public Function: connectNode(Node*)
 * -----------------------------------
 * Connect this node to the given node.
 * This function decides whether this
 * node will be drawing the x or y section
 * of the wire, depending on relative positions.
 */
void Node::connectNode(Node *node)
{
    Connection cnx;
    cnx.node = node;
    cnx.line = new QGraphicsLineItem(0, 0, 0, 0, this);
    cnx.line->setPen(wire);
    cnx.line->setFlag(ItemStacksBehindParent);

    Connection thisCnx;
    thisCnx.node = this;
    thisCnx.line = new QGraphicsLineItem(0, 0, 0, 0, node);
    thisCnx.line->setPen(wire);
    thisCnx.line->setFlag(ItemStacksBehindParent);

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

void Node::displayID(int id)
{
    label = new QGraphicsSimpleTextItem(QString::number(id), this);
    label->setPos(1, -label->boundingRect().height() - 5);
}

QSet<Node *> Node::getConnectedElementNodes()
{
    QSet<Node *> nodes;
    QSet<Node *> seen;
    getConnectedNodes(this, &nodes, &seen);
    nodes.remove(this);
    return nodes;
}

void Node::getConnectedNodes(Node *node, QSet<Node *> *nodes, QSet<Node *> *seen)
{
    if (node->hasElement()) nodes->insert(node);
    seen->insert(node);
    for (Connection c : xNodes + yNodes) {
        if (!seen->contains(c.node))
            getConnectedNodes(c.node, nodes, seen);
    }
}

/* Public Function: removeXNode(Connection)
 * ----------------------------------------
 * Remove the given node connection from
 * the list of nodes for which this node
 * draws the horizontal wire section.
 */
void Node::removeXNode(Connection c)
{
    if (xNodes.indexOf(c) == -1) {
        qInfo() << "node not found";
        return;
    }
    Connection cnx = xNodes[xNodes.indexOf(c)];
    cnx.line->setLine(0, 0, 0, 0);
    xNodes.removeOne(cnx);
}

/* Public Function: removeYNode(Connection)
 * ----------------------------------------
 * Remove the given node connection from
 * the list of nodes for which this node
 * draws the vertical wire section.
 */
void Node::removeYNode(Connection c)
{
    if (yNodes.indexOf(c) == -1) {
        qInfo() << "node not found";
        return;
    }
    Connection cnx = yNodes[yNodes.indexOf(c)];
    cnx.line->setLine(0, 0, 0, 0);
    yNodes.removeOne(cnx);
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

// ============= EVENT HANDLERS =============================

/* Event: hoverEnterEvent(...)
 * ---------------------------
 * Show node on hover
 *
 */
void Node::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    showNode();
    QGraphicsItem::hoverEnterEvent(event);
}

/* Event: leaveEvent(...)
 * ---------------------------
 * Hide node after hover
 */
void Node::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    hideNode();
    QGraphicsItem::hoverLeaveEvent(event);
}

/* Item Change: itemChange(...)
 * ----------------------------
 * Update wires and tell connected nodes
 * to update their sections when the node moves.
 */
QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemScenePositionHasChanged) {
        for (Connection c : xNodes + yNodes)
            c.node->update();
    }
    update();
    return QGraphicsItem::itemChange(change, value);
}

/* Focus Event: focusInEvent(...)
 * ------------------------------
 * Set wire and line color to red.
 *
 * TODO: Tell connected nodes to do this too.
 */
void Node::focusInEvent(QFocusEvent *event)
{
    wire.setColor(Qt::red);
    line = QColor(Qt::red);
    showNode();
    update();
    QGraphicsItem::focusInEvent(event);
}

/* Focus Event: focusOutEvent(...)
 * -------------------------------
 * Set wire and line color to black.
 *
 * TODO: Tell connected nodes to do this too.
 */
void Node::focusOutEvent(QFocusEvent *event)
{
    wire.setColor(Qt::black);
    line = QColor(Qt::black);
    hideNode();
    update();
    QGraphicsItem::focusOutEvent(event);
}
