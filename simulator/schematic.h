#ifndef SCHEMATIC_H
#define SCHEMATIC_H

#include <QtWidgets>

#include "graphics/circuitelement.h"
#include "graphics/node.h"
#include "simulation/netlist.h"
#include "wizard/simulationwizard.h"
#include "simulation/spiceengine.h"

class Schematic : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit Schematic(QObject *parent = nullptr);
    enum Mode { Edit, Build, Draw };
    Q_ENUM(Mode)
    enum { NoStartError = 1, IncompleteError = 2 };

    void setMode(Mode mode) { this->mode = mode; }
    void setElementProperties(CircuitElement::ElementProperties properties,
                              QString shadowPath);

    void simulatePressed() { simulate(); }
    void deletePressed() { deleteSelection(); }
    void clearPressed() { deleteAll(); }
    void savePressed() { simulate(true); }

protected:
//    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void drawBackground(QPainter *painter, const QRectF &rect);
    void keyReleaseEvent(QKeyEvent *event);

signals:
    void schematicClicked(); // -> (MainWindow) slotSchematicClicked()
    void parseComplete(bool success);

public slots:
    void parseSchematic();

private:
    Mode mode;
    const int gridSize = 20;

    // Displaying elements
    QGraphicsPixmapItem *curShadow;
    QPixmap shadowImage;
    CircuitElement::ElementProperties elementProperties;

    // Track mouse
    QPointF curPos;
    qreal lastClickX, lastClickY;
    bool dragging = false;

    // Drawing wires
    Node *startNode;
    Node *activeNode;

    // Netlist & Simulator
    Netlist *netlist;
    QString netlistFilename;
    SimulationWizard *simulationOptions;
    SpiceEngine *spiceEngine;

    // Parsing
    void simulate(bool saveOnly = false);
    int _parse();
    int _parseFrom(Node *startNode, int startNodeID, int &curNodeID, CircuitElement *lastAdded, QSet<Node *>&seen);
    int showSimulationOptions();
    void removeNodeLabels();
    bool parseErrorFlag = false;

    // Drawing functions
    void addElement();
    void checkNodesForConnections(CircuitElement *element);
    void startDrawingWire();
    void stopDrawingWire(Node *endNode);
    void addWire(Node *endNode);
    QPointF gridPos(QPointF point);
    QPointF gridPos(qreal x, qreal y);
    void deleteSelection();
    void deleteAll();
};

#endif // SCHEMATIC_H
