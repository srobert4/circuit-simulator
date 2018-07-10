#include "schematic.h"

Schematic::Schematic(int x, int y, int width, int height, QWidget *parent)
    : QWidget(parent)
{
    setAcceptDrops(true);
    elements = new QVector<CircuitElement*>;
    nodes = new QVector<Node*>;
    setGeometry(x, y, width, height);
    connect(this, SIGNAL(schematicClicked()),
            parent, SLOT(slotSchematicClicked()));
}

void Schematic::mousePressEvent(QMouseEvent *event)
{
    x_last_click = event->x();
    y_last_click = event->y();
    emit schematicClicked();
}

void Schematic::addElement(QString path) {
    int width = 80;
    int height = 30;
    CircuitElement *elem = new CircuitElement(
                x_last_click - width / 2,
                y_last_click - height / 2,
                width, height, path, this);
    elements->append(elem);
    Node *node_1 = new Node(x_last_click - width / 2,
                            y_last_click,
                            this);
    Node *node_2 = new Node(x_last_click + width / 2,
                            y_last_click,
                            this);
    node_1->show();
    node_2->show();
    nodes->append(node_1);
    nodes->append(node_2);
}
