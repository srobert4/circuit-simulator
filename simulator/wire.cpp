#include "wire.h"

/* Constructor: Wire */
Wire::Wire(QGraphicsLineItem *line, Node *startNode, Node *endNode, int id, QGraphicsItem *parent) :
    SchematicItem(id, "wire", parent)
{
    setFlag(ItemIsFocusable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsScenePositionChanges);

    this->line = line->line();
    this->rect = line->boundingRect();
    this->startNode = startNode;
    this->endNode = endNode;
}

// ----------- PUBLIC FUNCTIONS ----------------

/* Public Function: paint(QPainter *, ...)
 * ---------------------------------------
 * Draws Wire.
 */
void Wire::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QPen pen = QPen();
    pen.setColor(Qt::black);
    pen.setWidth(2);
    if (isSelected())
        pen.setColor(Qt::red);

    painter->setPen(pen);
    painter->drawLine(this->line);
}

void Wire::setLine(QGraphicsLineItem *line)
{
    this->line = line->line();
    this->rect = line->boundingRect();
}


//QVariant Wire::itemChange(GraphicsItemChange change, const QVariant &value)
//{
//    if (change == QGraphicsItem::ItemScenePositionHasChanged)
//        line.setLine(0, 0, endNode->x() - startNode->x(), endNode->y() - startNode->y());
//    return QGraphicsItem::itemChange(change, value);
//}
