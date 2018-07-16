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
        setFlag(ItemIsSelectable);
        this->line = line->line();
        this->startNode = startNode;
        this->endNode = endNode;
    }

    // required functions
    QRectF boundingRect() const { return QRectF(line.p1(), line.p2()); }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        painter->setPen(Qt::black);
        if (isSelected())
            painter->setPen(Qt::red);

        painter->drawLine(this->line);
    }

    void setLine(QGraphicsLineItem *line) { this->line = line->line(); }

protected:
    void focusInEvent(QFocusEvent *event) {
        qInfo() << "wire focused";
        QGraphicsItem::focusInEvent(event);
    }
    void focusOutEvent(QFocusEvent *event) {
        QGraphicsItem::focusInEvent(event);
    }

private:
    QLineF line;
    int id;
    Node *startNode;
    Node *endNode;
};

#endif // WIRE_H
