#ifndef WIRE_H
#define WIRE_H

#include <QtWidgets>
#include "node.h"

class Wire
{
public:
    Wire(QGraphicsLineItem *line,
        Node *startNode, Node *endNode, int id)
    {
        this->line = line;
        line->setZValue(-1);
        this->id = id;
        this->startNode = startNode;
        this->endNode = endNode;
    }

private:
    QGraphicsLineItem *line;
    int id;
    Node *startNode;
    Node *endNode;
};

#endif // WIRE_H
