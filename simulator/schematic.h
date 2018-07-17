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
    enum Mode { Edit, Build, Draw };
    Q_ENUM(Mode)

    void setMode(Mode mode) { this->mode = mode; }
    void setImagePaths(QString &imgPath, QString &selectedPath, QString &dragPath);

protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void drawBackground(QPainter *painter, const QRectF &rect);
    void keyReleaseEvent(QKeyEvent *event);

signals:
    void schematicClicked(); // -> (MainWindow) slotSchematicClicked()

private:
    Mode mode;

    // Track components
    QMap<int, CircuitElement*> elements;
    QMap<int, Wire*> wires;
    int elemId, nodeId, wireId;

    // Displaying elements
    const int elementWidth = 160;
    void addElement();
    QGraphicsPixmapItem *curShadow;
    int lastClickX, lastClickY;
    QPixmap image;
    QPixmap selectedImage;
    QPixmap shadowImage;

    // Track mouse
    QPointF curPos;

    // Drawing wires
    Node *startNode;
    Node *activeNode;

    const int gridSize = 20;


    // Drawing functions
    void startDrawingWire();
    void stopDrawingWire(Node *endNode);
    void addWire(Node *endNode);
    QPointF gridPos(QPointF point);
    QPointF gridPos(qreal x, qreal y);
};

#endif // SCHEMATIC_H
