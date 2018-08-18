#ifndef SCHEMATIC_H
#define SCHEMATIC_H

#include <QtWidgets>

#include "graphics/circuitelement.h"
#include "graphics/node.h"
#include "simulation/netlist.h"
#include "wizard/simulationwizard.h"
#include "simulation/spiceengine.h"

/* CLASS: Schematic
 * ================
 * Inherits: QGraphicsScene
 * Parent: MainWindow
 * Children:
 *  - CircuitElement
 *  - Node
 *  - SimulationWizard
 *  - Netlist
 *
 * The Schematic contains the bulk of the program logic. It is responsible
 * for all drawing and parsing of the diagram. It launches the SimulationWizard
 * in response to toolbar events.
 */

class Schematic : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit Schematic(QObject *parent = nullptr);
    /* Enum: Mode
     * ----------
     * The Schematic can be in one of three modes:
     *  - Edit: The default, most common mode. No element is currently being
     *          placed and no wire is being drawn.
     *  - Build: An ElementSelector button is selected and the shadow image
     *           is being shown at the mouse to show that teh user is choosing
     *           where to place the element.
     *  - Draw: A node has been clicked on and the user is drawing a wire.
     *
     * The mode is changed by the Schematic itself and by the ElementSelector
     * via the public method setMode()
     */
    enum Mode { Edit, Build, Draw };
    Q_ENUM(Mode)
    /* Errors:
     * -------
     * - NoStartError: Returned by parsing functions if the diagram does not
     *                 contain a starting ground element
     * - IncompleteError: Returned by parsing functions if the diagram has a
     *                    dangling node not connected to any other node
     */
    enum { NoStartError = 1, IncompleteError = 2 };

    // setMode() and setElementProperties() are called by the ElementSelector
    // when a button is selected
    void setMode(Mode mode) { this->mode = mode; }
    void setElementProperties(CircuitElement::ElementProperties properties,
                              QString shadowPath);

    // The following functions are called when the corresponding toolbar action
    // is triggered
    void simulatePressed() { simulate(); }
    void deletePressed() { deleteSelection(); }
    void clearPressed() { deleteAll(); }
    void savePressed() { simulate(true); }

protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void drawBackground(QPainter *painter, const QRectF &rect);
    void keyReleaseEvent(QKeyEvent *event);

signals:
    void schematicClicked(); // -> ElementSelector
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
    int _parseFrom(Node *startNode, int startNodeID, int &curNodeID,
                   CircuitElement *lastAdded, QSet<Node *>&seen);
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
