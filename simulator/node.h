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
        int elemId,
        QGraphicsItem *parent = nullptr
    );

    // required functions
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    // getters and setters
    void setId(int id) { this->id = id; }
    void setElemId(int elem) { elemId = elem; }
    void addWire(int wire) { wires.append(wire); }

    // display functions
    void showNode();
    void hideNode();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    int rad;
    int id, elemId; // track self and associated element
    QVector<int> wires; // track wires beginning or ending at this node
    QColor line;
    QColor fill;

};

#endif // NODE_H
