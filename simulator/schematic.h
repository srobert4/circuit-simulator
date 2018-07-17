#ifndef SCHEMATIC_H
#define SCHEMATIC_H

#include <QtWidgets>

#include "circuitelement.h"
#include "node.h"

class Schematic : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit Schematic(QObject *parent = nullptr);
    enum Mode { Edit, Build, Draw };
    Q_ENUM(Mode)

    void setMode(Mode mode) { this->mode = mode; }
    void setElementProperties(CircuitElement::ElementProperties &properties,
                              QString &shadowPath);

protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void drawBackground(QPainter *painter, const QRectF &rect);
    void keyReleaseEvent(QKeyEvent *event);

signals:
    void schematicClicked(); // -> (MainWindow) slotSchematicClicked()

private:
    Mode mode;
    const int gridSize = 20;

    // Displaying elements
    QGraphicsPixmapItem *curShadow;
    QPixmap shadowImage;
    CircuitElement::ElementProperties elementProperties;

    // Track mouse
    QPointF curPos;
    int lastClickX, lastClickY;

    // Drawing wires
    Node *startNode;
    Node *activeNode;

    // Drawing functions
    void addElement();
    void startDrawingWire();
    void stopDrawingWire(Node *endNode);
    void addWire(Node *endNode);
    QPointF gridPos(QPointF point);
    QPointF gridPos(qreal x, qreal y);
};

#endif // SCHEMATIC_H
