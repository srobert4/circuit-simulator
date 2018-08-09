#include "schematic.h"

/* Constructor: Schematic(QWidget *)
 * ---------------------------------
 * Initialize to edit mode.
 * Initialize simulation engine.
 */
Schematic::Schematic(QObject *parent)
    : QGraphicsScene(parent)
{
    netlist = nullptr;
    curShadow = nullptr;
    simulationOptions = nullptr;
    mode = Schematic::Edit;
}

// ============ PUBLIC FUNCTIONS ===============================================

/* Public Function: setElementProperties(...)
 * ------------------------------------------------------------
 * Called by mainWindow to set the element properties for the
 * selected element. Store properties for use in addElement()
 *
 * imagePath - normal black image
 * selectedPath - red image shown when element is selected
 * shadowPath - grey image shown when image is being placed
 */
void Schematic::setElementProperties(CircuitElement::ElementProperties properties,
                                     QString shadowPath)
{
    elementProperties = properties;
    shadowImage = QPixmap(shadowPath);
    shadowImage = shadowImage.scaledToWidth(properties.image.width());
    if (curShadow){
        removeItem(curShadow);
        delete curShadow;
        curShadow = nullptr;
    }
}


// =================== PRIVATE FUNCTIONS =======================================

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

    addItem(elem);
    checkNodesForConnections(elem);
    setFocusItem(elem);
}

/* Private Function: checkNodesForConnections(CircuitElement *)
 * ------------------------------------------------------------
 * Check if the nodes of the given element are overlapping
 * with any other nodes. If they are then connect overlapping
 * nodes.
 */
void Schematic::checkNodesForConnections(CircuitElement *element)
{
    // connect to elements if nodes on top of one enother
    Node *nodeOne = element->getNodeOne();
    foreach(QGraphicsItem *item, collidingItems(nodeOne)) {
        if (item->type() == Node::Type && item != nodeOne) {
            Node *node = qgraphicsitem_cast<Node *>(item);
            nodeOne->connectNode(node);
        }
    }

    Node *nodeTwo = element->getNodeTwo();
    foreach(QGraphicsItem *item, collidingItems(nodeTwo)) {
        if (item->type() == Node::Type && item != nodeTwo)
            nodeTwo->connectNode(qgraphicsitem_cast<Node *>(item));
    }
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
        removeItem(activeNode);
        delete activeNode;
        startNode->connectNode(endNode);
    } else {
        activeNode->setPos(gridPos(activeNode->scenePos()));
    }
    startNode = nullptr;
    activeNode = nullptr;
}

/* Private Function: gridPos(qreal x, qreal y)
 * -------------------------------------------
 * Returns the grid position closest to the given point (x,y).
 */
QPointF Schematic::gridPos(qreal x, qreal y)
{
    int newX = int(x) - (int(x) % gridSize);
    if (int(x) % gridSize > gridSize / 2) newX += gridSize;
    int newY = int(y) - (int(y) % gridSize);
    if (int(y) % gridSize > gridSize / 2) newY += gridSize;
    return QPointF(newX, newY);
}

/* Private Function: gridPos(QPointF)
 * ----------------------------------
 * Overloaded convenience function.
 */
QPointF Schematic::gridPos(QPointF point)
{
    return gridPos(point.x(), point.y());
}

/* Privat Functino: simulate(bool)
 * -------------------------------
 * Initialize a new netlist and
 * carry out simulation (bool = false)
 * or save.
 */
void Schematic::simulate(bool saveOnly)
{
    // Construct netlist
    if (netlist != nullptr) delete netlist;
    netlist = new Netlist();

    // if saveOnly then parse the schematic since
    // it will definitely be used.
    if (saveOnly) parseSchematic();

    if (parseErrorFlag) {
        removeNodeLabels();
        parseErrorFlag = false;
        return;
    }

    // Show options dialog box
    simulationOptions = new SimulationWizard(netlist, saveOnly);
    connect(simulationOptions, &SimulationWizard::parseCircuit,
            this, &Schematic::parseSchematic);
    if (!parseErrorFlag) {
        connect(this, &Schematic::parseComplete, [=](bool success){
            if (simulationOptions && !success) simulationOptions->close();
        });
        simulationOptions->exec();
    }

    delete simulationOptions;
    simulationOptions = nullptr;
    removeNodeLabels();
    parseErrorFlag = false;
}

/* Private Function: parse()
 * -------------------------
 * Start the parsing process for
 * the currently drawn circuit. Finds
 * starting node and starts recursion.
 *
 * Returns 0 on success or non-zero error
 * value
 *
 * This function is called in the slot parseSchematic
 * which handles errors. It should not be called on its own
 */
int Schematic::_parse()
{
    CircuitElement *start = nullptr;
    QList<QGraphicsItem *> items = this->items();
    foreach(QGraphicsItem *item, items) {
        if (item->type() == CircuitElement::Type) {
            CircuitElement *element = qgraphicsitem_cast<CircuitElement *>(item);
            if (element->getSubtype() == "start ground") {
                start = element;
                break;
            }
        }
    }
    if (start == nullptr) return NoStartError;

    int nodeID = -1;
    Node *startNode = start->getNodeTwo(); // TODO: don't hardcode
    QSet<Node *> seen;
    return _parseFrom(startNode, 0, nodeID, nullptr, seen);
}

/* Private Function: parseFrom(Node *, int, int, CircuitElement *, QSet<Node *>)
 * -----------------------------------------------------------------------------
 * Recursive parsing function to parse currently drawn schematic.
 * Returns 0 on success or non-zero error value.
 *
 * This function is called by _parse() and should not be used on its own
 */
int Schematic::_parseFrom(Node *startNode,
                          int startNodeID,
                          int &curNodeID,
                          CircuitElement *lastAdded,
                          QSet<Node *> &seen)
{
    int ret;
    if (startNode == nullptr) return -1;

    if (startNode->hasElement() && curNodeID != -1) {
        CircuitElement *element =
                qgraphicsitem_cast<CircuitElement *>(startNode->getElement());

        if (element->getSubtype() == "ground") {
            if (lastAdded == nullptr) return IncompleteError;
            ret = netlist->groundElement(lastAdded);
            if (ret != 0) {
                return ret;
            }
            startNode->displayID(0);
//            update();
//            QApplication::processEvents();
//            std::this_thread::sleep_for (std::chrono::seconds(1));
            curNodeID--;
            return 0;
        }

        curNodeID++;
        ret = netlist->addElement(element, startNodeID, curNodeID);
        if (ret != 0) return ret;
        lastAdded = element;
        startNode->displayID(startNodeID);

//        update();

//        QApplication::processEvents();
//        std::this_thread::sleep_for (std::chrono::seconds(1));
        seen.insert(startNode);
        startNode = element->getOtherNode(startNode);
    }

    if (curNodeID == -1) curNodeID = 0;
    seen.insert(startNode);
    startNodeID = curNodeID;
    Node *node;
    int connections = 0;
    foreach (node, startNode->getConnectedNodes()) {
        if (seen.contains(node)) continue;
        connections++;
        ret = _parseFrom(node, startNodeID, curNodeID, lastAdded, seen);
        if (ret != 0) return ret;
    }
    if (connections == 0) return IncompleteError;
    return 0;
}

/* Private Function: removeNodeLabels()
 * ------------------------------------
 * Remove all node labels from the schematic
 */
void Schematic::removeNodeLabels()
{
    foreach(QGraphicsItem *item, items()) {
        if (item->type() == Node::Type){
            Node *node = qgraphicsitem_cast<Node *>(item);
            node->hideID();
        }
    }
}

/* Private Function: deleteAll()
 * -----------------------------
 * Deletes all items in the scene
 */
void Schematic::deleteAll()
{
    foreach(QGraphicsItem *item, items())
        item->setSelected(true);
    deleteSelection();
}

/* Private Function: deleteSelection()
 * -----------------------------------
 * Delete selected graphics items
 */
void Schematic::deleteSelection()
{
    QList<QGraphicsItem *> toDelete;
    foreach(QGraphicsItem *item, selectedItems()) {
        if (item->type() == CircuitElement::Type ||
                (item->type() == Node::Type &&
                 !qgraphicsitem_cast<Node *>(item)->hasElement())) {
           toDelete.append(item);
        }
    }

    foreach(QGraphicsItem *item, toDelete) {
        toDelete.removeOne(item);
        removeItem(item);
        delete item;
    }
}

// ==================== SLOTS ==================================================

/* Slot: parseSchematic()
 * ----------------------
 * Function to parse schematic and handle error cases.
 * Emits parseComplete(success) to let simulationWizard know that
 * parsing is complete.
 */
void Schematic::parseSchematic()
{
    // Parse diagram and add elements
    int ret = _parse();
    if (ret != 0) {
        QString errormsg;
        switch (ret) {
        case Schematic::NoStartError:
            errormsg = "Your circuit must contain a starting ground element.";
            break;
        case Netlist::NoNameError:
            errormsg = "All elements must be named.";
            break;
        case Netlist::NoValueError:
            errormsg = "All elements must have a value assigned.";
            break;
        case Schematic::IncompleteError:
            errormsg = "Incomplete circuit.";
            break;
        case Netlist::DuplicateNameError:
            errormsg = "Duplicate element names. All element names must be unique.";
            break;
        case -1:
            errormsg = "Unknown error occurred.";
        }
        QMessageBox *box = new QMessageBox(QMessageBox::Critical, "Parsing Error", errormsg);
        box->exec();
        delete box;
        removeNodeLabels();
        parseErrorFlag = true;
    }
    emit parseComplete(!parseErrorFlag);
}

// ================== EVENT HANDLERS ===========================================

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
    switch( mode )
    {
    case Schematic::Build:
    {
        // store click position
        lastClickX = event->scenePos().x();
        lastClickY = event->scenePos().y();

        // stop displaying shadow
        removeItem(curShadow);
        delete curShadow;
        curShadow = nullptr;

        // add element
        addElement();
        mode = Schematic::Edit;

        // tell selector to deselect all
        emit schematicClicked();
        break;
    }

    case Schematic::Draw:
    {
        // Find out if stopped on a node or not
        Node *node = nullptr;
        foreach (QGraphicsItem *it, items(event->scenePos())) {
            if (it->type() == Node::Type && it != activeNode){
                node = qgraphicsitem_cast<Node *>(it);
                break;
            }
        }
        // Place wire and exit drawing mode
        stopDrawingWire(node);
        mode = Schematic::Edit;
        break;
    }

    case Schematic::Edit:
    {
        // If we didn't click on anything, do nothing
        QGraphicsItem *it = itemAt(event->scenePos(), QTransform());
        if (!it) break;

        // Either start drawing or move an element
        if (it->type() == Node::Type) {
           startNode = qgraphicsitem_cast<Node *>(it);
           startDrawingWire();
           mode = Schematic::Draw;
        } else if (it->type() == CircuitElement::Type) {
            foreach(QGraphicsItem *graphicsIt, selectedItems()) {
                graphicsIt->setPos(gridPos(graphicsIt->pos())); // snap all selected items to grid
                if (graphicsIt->type() == CircuitElement::Type)
                    checkNodesForConnections(qgraphicsitem_cast<CircuitElement *>(graphicsIt));
            }
        }

        break;
    }
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

/* MouseEvent: mouseMoveEvent(QMouseEvent *)
 * ----------------------------------------
 * Build mode:
 *      - move shadow
 * Draw mode:
 *      - move activeNode
 * Edit mode:
 *      - delete shadow if exists
 */
void Schematic::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    curPos = event->scenePos();
    switch( mode )
    {
    case Schematic::Build:
    {
        QGraphicsItem *it;
        foreach (it, selectedItems())
            it->setSelected(false);
        if (curShadow == nullptr) {
            curShadow = addPixmap(shadowImage);
            curShadow->setOffset(-shadowImage.width() / 2, -shadowImage.height() / 2);
            curShadow->setFlag(QGraphicsItem::ItemIsMovable);
        }
        curShadow->setPos(curPos);
        break;
    }
    case Schematic::Draw:
        activeNode->setPos(curPos);
        break;
    case Schematic::Edit:
        if (curShadow != nullptr) {
            removeItem(curShadow);
            delete curShadow;
            curShadow = nullptr;
        }
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
 * Right arrow - rotate selected items cw
 * Left arrow - rotate selected items ccw
 * Esc - deselect all items, stop drawing wire if drawing
 */
void Schematic::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case  Qt::Key_Delete:
        deleteSelection();
        break;

    case Qt::Key_Backspace:
        deleteSelection();
        break;

    case Qt::Key_Escape:
        if (mode == Schematic::Draw) {
            removeItem(activeNode);
            delete activeNode;
            activeNode = nullptr;
            startNode = nullptr;
            mode = Schematic::Edit;
        }
        for (auto it : selectedItems())
            it->setSelected(false);
        break;

    case Qt::Key_Left:
        foreach(QGraphicsItem *item,  selectedItems()) {
            if (item->type() == CircuitElement::Type) {
                qgraphicsitem_cast<CircuitElement *>(item)->rotate(-90);
            }
        }
        break;
    case Qt::Key_Right:
        foreach(QGraphicsItem *item,  selectedItems()) {
            if (item->type() == CircuitElement::Type) {
                qgraphicsitem_cast<CircuitElement *>(item)->rotate(90);
            }
        }
        break;
    }
    QGraphicsScene::keyReleaseEvent(event);
}
