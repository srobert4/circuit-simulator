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
    void setImagePaths(QString &imgPath, QString &dragPath);

protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

signals:
    void schematicClicked(); // -> (MainWindow) slotSchematicClicked()

private:
    Mode mode;

    // Track components
    QMap<int, CircuitElement*> elements;
    QMap<int, Wire*> wires;
    int elemId, nodeId, wireId;

    const int TypeKey = 0;
    const int IDKey = 1;

    // Displaying elements
    void addElement(QString path);
    QGraphicsPixmapItem *curShadow;
    int lastClickX, lastClickY;
    QString imagePath;
    QPixmap shadowImage;

    // Track mouse
    QPointF curPos;

    // Drawing wires
    QGraphicsLineItem *curWire;
    Node *activeNode;
    QPointF startPos;


    // Drawing functions
    void startDrawingWire();
    void stopDrawingWire(Node *endNode);
    void addWire(Node *endNode);
};

#endif // SCHEMATIC_H
