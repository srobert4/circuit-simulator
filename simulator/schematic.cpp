#include "schematic.h"

/* Constructor: Schematic(QWidget *)
 * ---------------------------------
 * Initialize to edit mode.
 */
Schematic::Schematic(QObject *parent)
    : QGraphicsScene(parent)
{
    netlist = nullptr;
    curShadow = nullptr;
    mode = Schematic::Edit;
}

// ============ PUBLIC FUNCTIONS ================================

/* Public Function: setElementProperties(...)
 * ------------------------------------------------------------
 * Called by mainWindow to set the element properties for the
 * selected element. Store properties for use in addElement()
 *
 * imagePath - normal black image
 * selectedPath - red image shown when element is selected
 * shadowPath - grey image shown when image is being placed
 */
void Schematic::setElementProperties(CircuitElement::ElementProperties &properties, QString &shadowPath)
{
    elementProperties = properties;
    shadowImage = QPixmap(shadowPath);
    shadowImage = shadowImage.scaledToWidth(properties.image.width());
}

void Schematic::simulate()
{
    // Construct netlist
    if (netlist != nullptr) delete netlist;
    netlist = new Netlist();

    // Parse diagram and add elements
    parse();

    // Show options dialog box
    simulationOptions = new SimulationOptionsDialog(netlist);
    if(simulationOptions->exec() == QDialog::Rejected) return;
    simulationOptions->processInput();

    // Write netlist to file
    netlist->writeToFile(simulationOptions->getFilename());
    qInfo() << "Written to file: " << simulationOptions->getFilename();

    // Get command and display in dialog box
//    QString command = netlist->getCommand();
//    qInfo() << "Run simulation with: " << command;
    delete simulationOptions;
    simulationOptions = nullptr;
}

// ============ PRIVATE FUNCTIONS ================================

/* Private Function: addElement()
 * ------------------------------------
 * Adds a new element to the Schematic centered at
 * the last point clicked with the mouse, with the currently
 * selected image (stored at image).
 *
 * This function also creates two nodes, one for each
 * end of the element.
 */
void Schematic::addElement() {
    CircuitElement *elem = new CircuitElement(elementProperties);
    elem->setPos(gridPos(lastClickX, lastClickY));

    Node *nodeOne = new Node(elem, elem);
    Node *nodeTwo = new Node(elem, elem);
    nodeOne->setPos(QPointF(-elem->width() / 2, 0));
    nodeTwo->setPos(QPointF(elem->width() / 2, 0));

    elem->setNodes(nodeOne, nodeTwo);

    addItem(elem);
    setFocusItem(elem);
}

/* Private Function: startDrawingWire()
 * -----------------------------------------------
 * Creates a new node to be dragged around and
 * connects it to the starting node.
 */
void Schematic::startDrawingWire()
{
    activeNode = new Node();
    activeNode->setPos(startNode->scenePos());
    startNode->connectNode(activeNode);
    addItem(activeNode);
}

/* Private Function: stopDrawingWire(Node*)
 * ----------------------------------------------
 * Snap node to grid or to existing end node.
 * Clear startNode and activeNode instance vars.
 */
void Schematic::stopDrawingWire(Node *endNode)
{
    if (endNode != nullptr) {
        startNode->connectNode(endNode);
        delete activeNode;
    } else {
        activeNode->setPos(gridPos(activeNode->scenePos()));
    }
    startNode = NULL;
    activeNode = NULL;
}

/* Private Function: gridPos(qreal x, qreal y)
 * -------------------------------------------
 * Returns the grid position at the upper diagonal
 * to the given point (x,y).
 *
 * TODO: snap to closest point.
 */
QPointF Schematic::gridPos(qreal x, qreal y)
{
    return QPointF(int(x) - (int(x) % gridSize), int(y) - (int(y) % gridSize));
}

/* Private Function: gridPos(QPointF)
 * ----------------------------------
 * Overloaded convenience function.
 */
QPointF Schematic::gridPos(QPointF point)
{
    return gridPos(point.x(), point.y());
}

void Schematic::parse()
{
    CircuitElement *start = nullptr;
    for(auto item : items()) {
        SchematicItem *schemIt = qgraphicsitem_cast<SchematicItem *>(item);
        if (schemIt->getSubtype() == "ground") {
            start = qgraphicsitem_cast<CircuitElement *>(item);
            break;
        }
    }
    if (start == nullptr) return; // throw error

    int nodeID = 0;
    Node *startNode = start->getNodeOne();
    parseFrom(startNode, nodeID);
}

void Schematic::parseFrom(Node *startNode, int &curNodeID)
{
    if (startNode == nullptr) return; // TODO - get angry
    int startNodeID = curNodeID;
    for (Node *node : startNode->getConnectedElementNodes()) {
        CircuitElement *element = qgraphicsitem_cast<CircuitElement *>(node->getElement());
        if(element->getSubtype() == "ground") continue;

        curNodeID = (grounded(element->getOtherNode(node)) ? 0 : curNodeID + 1);
        netlist->addElement(element, startNodeID, curNodeID);
        startNode->displayID(startNodeID);
        parseFrom(element->getOtherNode(node), curNodeID);
    }
}

bool Schematic::grounded(Node *node)
{
    for (Node *n : node->getConnectedElementNodes()) {
        CircuitElement *element = qgraphicsitem_cast<CircuitElement *>(n->getElement());
        if (element->getSubtype() == "ground") return true;
    }
    return false;
}

// ============= EVENT HANDLERS ================================

/* MouseEvent: mouseReleaseEvent(QMouseEvent *)
 * -------------------------------------------
 * Mode: Build
 *    - Add element
 * Mode: Draw
 *    - Place node
 * Mode: Edit
 *    - Move item or start drawing
 */
void Schematic::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    SchematicItem *item = nullptr;
    switch( mode )
    {
    case Schematic::Build:
        // store click position
        lastClickX = event->scenePos().x();
        lastClickY = event->scenePos().y();

        // stop displaying shadow
        removeItem(curShadow);
        delete curShadow;
        curShadow = NULL;

        // add element
        addElement();
        mode = Schematic::Edit;

        // tell selector to deselect all
        emit schematicClicked();
        break;

    case Schematic::Draw:
        // Find out if stopped on a node or not
        for (auto it : items(event->scenePos())) {
            SchematicItem *schemIt = qgraphicsitem_cast<SchematicItem *>(it);
            if (schemIt != activeNode && schemIt->getType() == "node"){
                item = schemIt;
                break;
            }
        }
        // Place wire and exit drawing mode
        stopDrawingWire(qgraphicsitem_cast<Node *>(item));
        mode = Schematic::Edit;
        break;

    case Schematic::Edit:
        // If we didn't click on anything, do nothing
        item = qgraphicsitem_cast<SchematicItem *>(itemAt(event->scenePos(), QTransform()));
        if (!item) break;

        // Either start drawing or move an element
        if (item->getType() == "node") {
           startNode = qgraphicsitem_cast<Node *>(item);
           startDrawingWire();
           mode = Schematic::Draw;
        } else if (item->getType() == "element") {
            for (auto it : selectedItems()) {
                it->setPos(gridPos(it->pos())); // snap all selected items to grid
            }
        }

        break;
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

/* MouseEvent: mouseMoveEvent(QMouseEvent *)
 * ----------------------------------------
 * Build mode:
 *      - move shadow
 * Draw mode:
 *      - move activeNode
 * Edit mode: nothing
 */
void Schematic::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    curPos = event->scenePos();
    switch( mode )
    {
    case Schematic::Build:
        if (curShadow == NULL) {
            curShadow = addPixmap(shadowImage);
            curShadow->setOffset(-shadowImage.width() / 2, -shadowImage.height() / 2);
            curShadow->setFlag(QGraphicsItem::ItemIsMovable);
        }
        curShadow->setPos(curPos);
        break;
    case Schematic::Draw:
        activeNode->setPos(curPos);
        break;
    case Schematic::Edit:
        break;
    }
    QGraphicsScene::mouseMoveEvent(event);
}

/* PaintEvent: drawBackground(QPainter *, const QRectF)
 * ----------------------------------------------------
 * Draw the grid background.
 */
void Schematic::drawBackground(QPainter *painter, const QRectF &rect)
{
    QPen pen = QPen(painter->pen());
    pen.setColor(Qt::gray);
    painter->setPen(pen);

    qreal left = int(rect.left()) - (int(rect.left()) % gridSize);
    qreal top = int(rect.top()) - (int(rect.top()) % gridSize);

    QVarLengthArray<QLineF, 100> lines;

    for (qreal x = left; x < rect.right(); x += gridSize)
        lines.append(QLineF(x, rect.top(), x, rect.bottom()));
    for (qreal y = top; y < rect.bottom(); y += gridSize)
        lines.append(QLineF(rect.left(), y, rect.right(), y));

    painter->drawLines(lines.data(), lines.size());
}

/* KeyEvent: keyReleaseEvent(QKeyEvent *)
 * --------------------------------------
 * Delete - delete selected items
 * r/right arrow - rotate selected items cw // TODO
 * R/left arrow - rotate selected items ccw // TODO
 * Esc - deselect all items, stop drawing wire if drawing
 */
void Schematic::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case  Qt::Key_Delete:
        for (auto it : selectedItems()) {
            SchematicItem *schemIt = qgraphicsitem_cast<SchematicItem *>(it);
            if (schemIt->getType() != "")
                delete it;
        }
        break;

    case Qt::Key_Backspace:
        for (auto it : selectedItems()) {
            SchematicItem *schemIt = qgraphicsitem_cast<SchematicItem *>(it);
            if (schemIt->getType() != "")
                delete it;
        }
        break;

    case Qt::Key_Escape:
        if (mode == Schematic::Draw) {
            delete activeNode;
            activeNode = NULL;
            startNode = NULL;
            mode = Schematic::Edit;
        }
        for (auto it : selectedItems())
            it->setSelected(false);
        break;
    }
    QGraphicsScene::keyReleaseEvent(event);
}
