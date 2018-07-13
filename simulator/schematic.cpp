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
    curWire = NULL;

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
void Schematic::setImagePaths(QString &imgPath, QString &dragPath)
{
    imagePath = imgPath;
    shadowImage = QPixmap(dragPath);
    shadowImage = shadowImage.scaled(160, 25);
}

// ----------- PRIVATE FUNCTIONS -----------------------------------------------

void Schematic::addElement(QString path) {
    int width = 160;
    int height = 24;

    // construct CircuitElement
    int id = elemId++;
    int node1Id = nodeId++;
    int node2Id = nodeId++;
    CircuitElement *elem = new CircuitElement(
                width, height, id, node1Id, node2Id, path,
                TypeKey, IDKey);
    elem->setPos(lastClickX, lastClickY);
    addItem(elem);
    elements[id] = elem;
}

/* Private Function: startDrawingWire(QPoint, int)
 * -----------------------------------------------
 * Sets start position and start node for a wire.
 * Sets drawing to true.
 */
void Schematic::startDrawingWire()
{
    curWire = new QGraphicsLineItem(0, 0, 0, 0);
    curWire->setPos(startPos);
    QPen pen = QPen(Qt::black);
    pen.setWidth(2);
    curWire->setPen(pen);
    addItem(curWire);
}

/* Private Function: stopDrawingWire(QPoint, int)
 * ----------------------------------------------
 * Sets drawing to false. Clears drawn wire.
 * Adds new Wire element corresponding to drawn wire.
 */
void Schematic::stopDrawingWire(Node *endNode)
{
    addWire(endNode);
    curWire = NULL;
    activeNode = NULL;
}

/* Private Function: addWire(QPoint, int)
 * --------------------------------------
 * Create a new wire. Gets start point from
 * startPos instance variable and end point
 * from argument. Gets start node from instance
 * variable activeNode and end node from argument.
 */
void Schematic::addWire(Node *endNode)
{
    if (endNode == NULL) {
        endNode = new Node(nodeId, -1, curWire);
        endNode->setPos(curWire->line().x2(), curWire->line().y2());
        endNode->setData(TypeKey, "node");
        endNode->setData(IDKey, nodeId);
    }
    curWire->setData(TypeKey, "wire");
    curWire->setData(IDKey, wireId);
    Wire *wire = new Wire(curWire,
                          activeNode,
                          endNode,
                          wireId);

    activeNode->addWire(wireId);
    endNode->addWire(wireId);
    wires[wireId++] = wire;
}

// ----------- EVENT HANDLERS -----------------------------------------------

/* MouseEvent: mousePressEvent(QMouseEvent *)
 * -------------------------------------------
 * If a wire is drawing, stop drawing it.
 * Else, save the position of the click and
 * ask the MainWindow for an element to add.
 */
void Schematic::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem *item = NULL;
    switch( mode )
    {
    case Schematic::Build:
        lastClickX = event->scenePos().x();
        lastClickY = event->scenePos().y();
        removeItem(curShadow);
        curShadow = NULL;
        addElement(imagePath);
        mode = Schematic::Edit;

        emit schematicClicked();
        break;

    case Schematic::Draw:
        for (auto it : items(event->scenePos())) {
            if (it->data(TypeKey) == "node"){
                item = it;
                curWire->setLine(0, 0, item->scenePos().x() - startPos.x(), item->scenePos().y() - startPos.y());
                break;
            }
        }
        stopDrawingWire(qgraphicsitem_cast<Node *>(item));
        mode = Schematic::Edit;
        break;

    case Schematic::Edit:
        item = itemAt(event->scenePos(), QTransform());
        if (!item) break;

        if (item->data(TypeKey).toString() == "node") {
           startPos = item->scenePos();
           activeNode = qgraphicsitem_cast<Node *>(item);
           startDrawingWire();
           mode = Schematic::Draw;

        } else if (item->data(TypeKey).toString() == "element") {
            item->setFocus();
        }
        break;
    }

    QGraphicsScene::mousePressEvent(event);
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
            curShadow->setOffset(-80, -12);
            curShadow->setFlag(QGraphicsItem::ItemIsMovable);
        }
        qInfo() << "curShadow: " << curShadow;
        qInfo() << "shadowImage: " << shadowImage;
        curShadow->setPos(curPos); //TODO: center image on cursor
        break;
    case Schematic::Draw:
        curWire->setLine(0, 0, curPos.x() - startPos.x(), curPos.y() - startPos.y());
        break;
    case Schematic::Edit:
        break;
    }
    QGraphicsScene::mouseMoveEvent(event);
}
