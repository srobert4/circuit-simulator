#ifndef WIRE_H
#define WIRE_H

#include <QtWidgets>
#include "node.h"

class Wire : SchematicItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit Wire(QGraphicsLineItem *line, Node *startNode, Node *endNode, int id, QGraphicsItem *parent = nullptr) :
        SchematicItem(id, "wire", parent)
    {
        setFlag(ItemIsFocusable);
        this->line = line;
        line->setZValue(-1);
        this->startNode = startNode;
        this->startNode->setZValue(1);
        this->endNode = endNode;
        this->endNode->setZValue(1);
        update();
    }

    // required functions
    QRectF boundingRect() const { return line->boundingRect(); }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        Q_UNUSED(painter);
        Q_UNUSED(option);
        Q_UNUSED(widget);
    }

protected:
    void focusInEvent(QFocusEvent *event) {
        qInfo() << "wire focused";
        line->setPen(QPen(Qt::red));
        line->update();
        QGraphicsItem::focusInEvent(event);
    }
    void focusOutEvent(QFocusEvent *event) {
        line->setPen(QPen(Qt::black));
        line->update();
        QGraphicsItem::focusInEvent(event);
    }

private:
    QGraphicsLineItem *line;
    int id;
    Node *startNode;
    Node *endNode;
};

#endif // WIRE_H
