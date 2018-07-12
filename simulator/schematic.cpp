#include "schematic.h"

/* Constructor: Schematic(QWidget *)
 * ---------------------------------
 * Allow mouse tracking so that wires
 * can be drawn without drag. Initialize
 * instance variables. Connect click signal
 * to parent slot.
 */
Schematic::Schematic(QObject *parent)
    : QGraphicsScene(parent)
{
    drawing = false;

    elemId = 0;
    nodeId = 0;
    wireId = 0;
}

// ----------- PUBLIC FUNCTIONS -----------------------------------------------

/* Public Function: addElement(QString)
 * ------------------------------------
 * Adds a new element to the Schematic centered at
 * the last point clicked with the mouse, with the
 * image at the given path.
 *
 * This function also creates two nodes, one for each
 * end of the element.
 *
 * The element and both nodes are given ID numbers and
 * tracked.
 */
void Schematic::addElement(QString path) {
    int width = 160;
    int height = 60;

    // construct CircuitElement
    int id = elemId++;
    int node1Id = nodeId++;
    int node2Id = nodeId++;
    CircuitElement *elem = new CircuitElement(
                width, height, id, node1Id, node2Id, path);
    elem->setPos(lastClickX, lastClickY);
    qInfo() << "Setting pos at (" << lastClickX << "," << lastClickY << ")";
    connect(elem->getNodeOne(), SIGNAL(nodeClicked(QPointF, int)),
            this, SLOT(slotNodeClicked(QPointF,int)));
    connect(elem->getNodeTwo(), SIGNAL(nodeClicked(QPointF, int)),
            this, SLOT(slotNodeClicked(QPointF,int)));
    addItem(elem);
    elements[id] = elem;
}

// ----------- PRIVATE FUNCTIONS -----------------------------------------------

/* Private Function: startDrawingWire(QPoint, int)
 * -----------------------------------------------
 * Sets start position and start node for a wire.
 * Sets drawing to true.
 */
void Schematic::startDrawingWire(QPointF start, int nodeId)
{
    drawing = true;
    startPos = start;
    activeNode = nodeId;

    curWire = new QGraphicsLineItem(0, 0, 0, 0);
    curWire->setPos(startPos);
    addItem(curWire);
}

/* Private Function: stopDrawingWire(QPoint, int)
 * ----------------------------------------------
 * Sets drawing to false. Clears drawn wire.
 * Adds new Wire element corresponding to drawn wire.
 */
void Schematic::stopDrawingWire(QPointF end, int nodeId)
{
    Q_UNUSED(end);
    Q_UNUSED(nodeId);
    drawing = false;
//    addWire(end);
    curWire = NULL;
}

/* Private Function: addWire(QPoint, int)
 * --------------------------------------
 * Create a new wire. Gets start point from
 * startPos instance variable and end point
 * from argument. Gets start node from instance
 * variable activeNode and end node from argument.
 */
//void Schematic::addWire(QPoint end, int endNode)
//{
//    curWire->setLine(0, 0, end.x() - startPos.x(), end.y() - startPos.y());
//    Wire *wire = new Wire(
//                    startPos,
//                    end,
//                    wireId,
//                    this
//                );

//    if (activeNode != -1) {
//        wire->setStartNode(activeNode);
//        nodes[activeNode]->addWire(wireId);
//    }

//    if (endNode != -1) {
//        wire->setEndNode(endNode);
//        nodes[endNode]->addWire(wireId);
//    }

//    activeNode = -1;
//    wires[wireId++] = wire;
//}

// ----------- SLOTS -----------------------------------------------

/* Slot: slotNodeClicked(QPoint, int)
 * ----------------------------------
 * Called when a node is clicked. Either
 * start or stop drawing a wire.
 */
void Schematic::slotNodeClicked(QPointF pos, int nodeId)
{
    if (drawing) {
        stopDrawingWire(pos, nodeId);
    } else {
        startDrawingWire(pos, nodeId);
    }
}

// ----------- EVENT HANDLERS -----------------------------------------------

/* MouseEvent: mousePressEvent(QMouseEvent *)
 * -------------------------------------------
 * If a wire is drawing, stop drawing it.
 * Else, save the position of the click and
 * ask the MainWindow for an element to add.
 */
void Schematic::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
//    qInfo() << "SCHEMATIC: Mouse press event";
    if (drawing) {
        stopDrawingWire(event->pos(), -1);
        return;
    }
    lastClickX = event->scenePos().x();
    lastClickY = event->scenePos().y();
    qInfo() << "Position in click handler: " << event->scenePos();
    emit schematicClicked();
    QGraphicsScene::mouseMoveEvent(event);
}

/* MouseEvent: mouseMoveEvent(QMouseEvent *)
 * ----------------------------------------
 * If drawing, update the wire to reach the
 * current mouse position.
 */
void Schematic::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!drawing) return;
    curPos = event->pos();
    curWire->setLine(0, 0, curPos.x() - startPos.x(), curPos.y() - startPos.y());
    QGraphicsScene::mouseMoveEvent(event);
}
