#include "schematic.h"

Schematic::Schematic(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    drawing = false;

    elemId = 0;
    nodeId = 0;
    wireId = 0;

    connect(this, SIGNAL(schematicClicked()),
            parent, SLOT(slotSchematicClicked()));
}

void Schematic::addElement(QString path) {
    int width = 160;
    int height = 60;
    // construct CircuitElement
    int id = elemId++;
    CircuitElement *elem = new CircuitElement(
                x_last_click - width / 2,
                y_last_click - height / 2,
                width, height, id, path, this);
    elements[id] = elem;

    // construct Nodes
    QPoint left, right;
    int node1Id = nodeId++;
    int node2Id = nodeId++;
    elem->getNodePosition(left, right);
    Node *node_1 = new Node(left.x(),
                            left.y(),
                            node1Id,
                            this);
    Node *node_2 = new Node(right.x(),
                            right.y(),
                            node2Id,
                            this);
    nodes[node1Id] = node_1;
    nodes[node2Id] = node_2;

    // link nodes and element
    elem->setNodeIds(node1Id, node2Id);
    node_1->setElemId(id);
    node_2->setElemId(id);

    // connect Nodes for drawing
    connect(node_1, SIGNAL(nodeClicked(QPoint, int)),
            this, SLOT(slotNodeClicked(QPoint, int)));
    connect(node_2, SIGNAL(nodeClicked(QPoint, int)),
            this, SLOT(slotNodeClicked(QPoint, int)));
}

void Schematic::startDrawingWire(QPoint start, int nodeId)
{
    drawing = true;
    start_pos = start;
    activeNode = nodeId;
}

void Schematic::stopDrawingWire(QPoint end, int nodeId)
{
    drawing = false;
    update();
    addWire(end, nodeId);
}

void Schematic::slotNodeClicked(QPoint clickPos, int nodeId)
{
    if (drawing) {
        stopDrawingWire(clickPos, nodeId);
    } else {
        startDrawingWire(clickPos, nodeId);
    }
}

void Schematic::addWire(QPoint end, int endNode)
{
    Wire *wire = new Wire(
                    start_pos,
                    end,
                    wireId,
                    this
                );

    if (activeNode != -1) {
        wire->setStartNode(activeNode);
        nodes[activeNode]->addWire(wireId);
    }

    if (endNode != -1) {
        wire->setEndNode(endNode);
        nodes[endNode]->addWire(wireId);
    }

    activeNode = -1;
    wires[wireId++] = wire;
}

void Schematic::mousePressEvent(QMouseEvent *event)
{
    if (drawing) {
        stopDrawingWire(event->pos(), -1);
        return;
    }
    x_last_click = event->x();
    y_last_click = event->y();
    emit schematicClicked();
}

void Schematic::mouseMoveEvent(QMouseEvent *event)
{
    if (!drawing) return;
    update();
    cur_pos = event->pos();
}


void Schematic::paintEvent(QPaintEvent *)
{
    if (!drawing) return;
    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.drawLine(start_pos, cur_pos);
}
