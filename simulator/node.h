#ifndef NODE_H
#define NODE_H

#include <QtWidgets>

class Node : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit Node(
        int id,
        QGraphicsItem *parent = nullptr
    );

    // required functions
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    // getters and setters
    void setElemId(int elem) { elemId = elem; }
    void addWire(int wire) { wires.append(wire); }

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *) override; // -> nodeClicked()

private:
    int rad;
    int id, elemId; // track self and associated element
    QVector<int> wires; // track wires beginning or ending at this node

signals:
    void nodeClicked(QPointF pos, int id); // -> (Schematic) slotNodeClicked()
};

#endif // NODE_H
