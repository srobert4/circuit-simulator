#ifndef NODE_H
#define NODE_H

#include <QtWidgets>
#include "schematicitem.h"

class Node : public SchematicItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

    struct Connection
    {
        Node *node;
        QGraphicsLineItem *line;

        bool operator ==(const Connection &c1) {
            return node == c1.node;
        }
    };

public:
    explicit Node(
            SchematicItem *element = nullptr,
            QGraphicsItem *parent = nullptr
            );
    ~Node();

    // required functions
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    void connectNode(Node *node);
    void connectToElement(SchematicItem *element);

    void addXNode(Node::Connection c) {xNodes.append(c);}
    void addYNode(Node::Connection c) {yNodes.append(c);}

    void removeXNode(Node::Connection c);
    void removeYNode(Node::Connection c);

    // display functions
    void showNode();
    void hideNode();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    const int rad = 5;
    QVector<Node::Connection> xNodes, yNodes; // we draw X for xNodes and Y for yNodes
    QColor line;
    QColor fill;
    QPen wire;
    SchematicItem *element;

};

#endif // NODE_H
