#include "schematic.h"

Schematic::Schematic(int x, int y, int width, int height, QWidget *parent)
    : QWidget(parent)
{
    setAcceptDrops(true);
    elements = new QVector<CircuitElement*>;
    setGeometry(x, y, width, height);
    connect(this, SIGNAL(schematicClicked()),
            parent, SLOT(slotSchematicClicked()));
}

void Schematic::mousePressEvent(QMouseEvent *event)
{
    pos_last_click = event->pos();
    emit schematicClicked();
}

void Schematic::addElement(QString path) {
    CircuitElement *elem = new CircuitElement(&pos_last_click, 30, 80, path, this);
    elements->append(elem);
}
