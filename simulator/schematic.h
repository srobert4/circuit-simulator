#ifndef SCHEMATIC_H
#define SCHEMATIC_H

#include <QtWidgets>

#include "circuitelement.h"
#include "symbol.h"
#include "node.h"
#include "wire.h"

class Schematic : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit Schematic(QObject *parent = nullptr);
    void addElement(QString path);
    bool isDrawing() { return drawing; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

signals:
    void schematicClicked(); // -> (MainWindow) slotSchematicClicked()

public slots:
    void slotNodeClicked(QPointF end, int nodeId); // <- (Node) nodeClicked()

private:
    // Track components
    QMap<int, CircuitElement*> elements;
    QMap<int, Node*> nodes;
    QMap<int, Wire*> wires;
    int elemId, nodeId, wireId;

    // Needed for drawing wires
    QGraphicsLineItem *curWire;
    int lastClickX, lastClickY;
    int activeNode;
    bool drawing;
    QPointF startPos;
    QPointF curPos;

    // Drawing functions
    void startDrawingWire(QPointF start, int nodeId);
    void stopDrawingWire(QPointF end, int nodeId);
    void addWire(QPointF end, int endNode = -1);
};

#endif // SCHEMATIC_H
