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
    elemId = 0;
    nodeId = 0;
    wireId = 0;

    curShadow = NULL;

    mode = Schematic::Edit;
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

// TODO: set pixmaps here with correct size and everything
// rather than using paths
void Schematic::setImagePaths(QString &imgPath, QString &selectedPath, QString &dragPath)
{
    image = QPixmap(imgPath);
    image = image.scaledToWidth(elementWidth);

    selectedImage = QPixmap(selectedPath);
    selectedImage = selectedImage.scaledToWidth(elementWidth);

    shadowImage = QPixmap(dragPath);
    shadowImage = shadowImage.scaledToWidth(elementWidth);
}

// ----------- PRIVATE FUNCTIONS -----------------------------------------------

void Schematic::addElement() {
    // construct CircuitElement
    int id = elemId++;
    int node1Id = nodeId++;
    int node2Id = nodeId++;
    CircuitElement *elem = new CircuitElement(id, node1Id, node2Id, image, selectedImage);
    elem->setPos(gridPos(lastClickX, lastClickY));

    Node *nodeOne = new Node(elem, elem);
    Node *nodeTwo = new Node(elem, elem);
    nodeOne->setPos(QPointF(-elem->width() / 2, 0));
    nodeTwo->setPos(QPointF(elem->width() / 2, 0));

    elem->setNodes(nodeOne, nodeTwo);

    addItem(elem);
    setFocusItem(elem);
    elements[id] = elem;
}

/* Private Function: startDrawingWire(QPoint, int)
 * -----------------------------------------------
 * Sets start position and start node for a wire.
 * Sets drawing to true.
 */
void Schematic::startDrawingWire()
{
    activeNode = new Node();
    activeNode->setPos(startNode->scenePos());
    startNode->connectNode(activeNode);
    addItem(activeNode);
}

/* Private Function: stopDrawingWire(QPoint, int)
 * ----------------------------------------------
 * Sets drawing to false. Clears drawn wire.
 * Adds new Wire element corresponding to drawn wire.
 */
void Schematic::stopDrawingWire(Node *endNode)
{
    if (endNode != nullptr) {
        startNode->connectNode(endNode);
        delete activeNode;
    }
    startNode = NULL;
    activeNode = NULL;
}

QPointF Schematic::gridPos(QPointF point)
{
    return gridPos(point.x(), point.y());
}

QPointF Schematic::gridPos(qreal x, qreal y)
{
    return QPointF(int(x) - (int(x) % gridSize), int(y) - (int(y) % gridSize));
    qreal px = (x < 0 ? x - 5 : x + 5);
    qreal py = (y < 0 ? y - 5 : y + 5);
    return QPointF(10 * (px / 10), 10 * (py / 10));
}

// ----------- EVENT HANDLERS -----------------------------------------------



/* MouseEvent: mouseReleaseEvent(QMouseEvent *)
 * -------------------------------------------
 * If a wire is drawing, stop drawing it.
 * Else, save the position of the click and
 * ask the MainWindow for an element to add.
 */
void Schematic::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    SchematicItem *item = nullptr;
    switch( mode )
    {
    case Schematic::Build:
        lastClickX = event->scenePos().x();
        lastClickY = event->scenePos().y();
        removeItem(curShadow);
        curShadow = NULL;
        addElement();
        mode = Schematic::Edit;

        emit schematicClicked();
        break;

    case Schematic::Draw:
        for (auto it : items(event->scenePos())) {
            SchematicItem *schemIt = qgraphicsitem_cast<SchematicItem *>(it);
            if (schemIt != activeNode && schemIt->getType() == "node"){
                item = schemIt;
                break;
            }
        }
        stopDrawingWire(qgraphicsitem_cast<Node *>(item));
        mode = Schematic::Edit;
        break;

    case Schematic::Edit:
        item = qgraphicsitem_cast<SchematicItem *>(itemAt(event->scenePos(), QTransform()));
        if (!item) break;

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
 * If drawing, update the wire to reach the
 * current mouse position.
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
 * Esc - deselect all items
 */
void Schematic::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case  Qt::Key_Delete:
        for (auto it : selectedItems()) {
            delete it;
        }
        break;

    case Qt::Key_Escape: // TODO check mode
        for (auto it : selectedItems())
            it->setSelected(false);
        break;
    }
    QGraphicsScene::keyReleaseEvent(event);
}
