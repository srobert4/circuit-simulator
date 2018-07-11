#include "schematic.h"

/* Constructor: Schematic(QWidget *)
 * ---------------------------------
 * Allow mouse tracking so that wires
 * can be drawn without drag. Initialize
 * instance variables. Connect click signal
 * to parent slot.
 */
Schematic::Schematic(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true); // allow mouseMoveEvent without dragging
    drawing = false;

    elemId = 0;
    nodeId = 0;
    wireId = 0;

    connect(this, SIGNAL(schematicClicked()),
            parent, SLOT(slotSchematicClicked())); // get element to be added
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
    CircuitElement *elem = new CircuitElement(
                lastClickX - width / 2,
                lastClickY - height / 2,
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

// ----------- PRIVATE FUNCTIONS -----------------------------------------------

/* Private Function: startDrawingWire(QPoint, int)
 * -----------------------------------------------
 * Sets start position and start node for a wire.
 * Sets drawing to true.
 */
void Schematic::startDrawingWire(QPoint start, int nodeId)
{
    drawing = true;
    startPos = start;
    activeNode = nodeId;
}

/* Private Function: stopDrawingWire(QPoint, int)
 * ----------------------------------------------
 * Sets drawing to false. Clears drawn wire.
 * Adds new Wire element corresponding to drawn wire.
 */
void Schematic::stopDrawingWire(QPoint end, int nodeId)
{
    drawing = false;
    update();
    addWire(end, nodeId);
}

/* Private Function: addWire(QPoint, int)
 * --------------------------------------
 * Create a new wire. Gets start point from
 * startPos instance variable and end point
 * from argument. Gets start node from instance
 * variable activeNode and end node from argument.
 */
void Schematic::addWire(QPoint end, int endNode)
{
    Wire *wire = new Wire(
                    startPos,
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

// ----------- SLOTS -----------------------------------------------

/* Slot: slotNodeClicked(QPoint, int)
 * ----------------------------------
 * Called when a node is clicked. Either
 * start or stop drawing a wire.
 */
void Schematic::slotNodeClicked(QPoint clickPos, int nodeId)
{
    if (drawing) {
        stopDrawingWire(clickPos, nodeId);
    } else {
        startDrawingWire(clickPos, nodeId);
    }
}

// ----------- EVENT HANDLERS -----------------------------------------------

/* MouseEvent: mousePressEvent(QMouseEvent *)
 * -------------------------------------------
 * If a wire is drawing, stop drawing it.
 * Else, save the position of the click and
 * ask the MainWindow for an element to add.
 */
void Schematic::mousePressEvent(QMouseEvent *event)
{
    if (drawing) {
        stopDrawingWire(event->pos(), -1);
        return;
    }
    lastClickX = event->x();
    lastClickY = event->y();
    emit schematicClicked();
}

/* MouseEvent: mouseMoveEvent(QMouseEvent *)
 * ----------------------------------------
 * If drawing, update the wire to reach the
 * current mouse position.
 */
void Schematic::mouseMoveEvent(QMouseEvent *event)
{
    if (!drawing) return;
    update();
    curPos = event->pos();
}

/* PaintEvent: paintEvent(QPaintEvent *)
 * -------------------------------------
 * If drawing, draw a line from instance
 * variable startPos to curPos.
 */
void Schematic::paintEvent(QPaintEvent *)
{
    if (!drawing) return;
    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.drawLine(startPos, curPos);
}
