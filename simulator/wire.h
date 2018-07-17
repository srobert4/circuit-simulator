#ifndef WIRE_H
#define WIRE_H

#include <QtWidgets>
#include "node.h"

class Wire : public SchematicItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit Wire(QGraphicsLineItem *line, Node *startNode, Node *endNode, int id, QGraphicsItem *parent = nullptr);

    // required functions
    QRectF boundingRect() const { return this->rect; }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setLine(QGraphicsLineItem *line);
    QLineF *getLine() { return &line; }

protected:
//    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    QRectF rect;
    QLineF line;
    int id;
    Node *startNode;
    Node *endNode;
};

#endif // WIRE_H
