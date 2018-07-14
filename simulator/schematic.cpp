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
        qInfo() << "creating a new node";
        endNode = new Node(nodeId, -1, curWire);
        endNode->setPos(curWire->line().x2(), curWire->line().y2());
    }

    Wire *wire = new Wire(curWire,
                          activeNode,
                          endNode,
                          wireId);

    activeNode->addWire(wireId);
    endNode->addWire(wireId);
    wires[wireId++] = wire;
    qInfo() << "Number of elements = " << items().length();
}

QPointF Schematic::gridPos(QPointF point)
{
    return QPointF(int(point.x()) - (int(point.x()) % gridSize), int(point.y()) - (int(point.y()) % gridSize));
    qreal px = (point.x() < 0 ? point.x() - 5 : point.x() + 5);
    qreal py = (point.y() < 0 ? point.y() - 5 : point.y() + 5);
    return QPointF(10 * (px / 10), 10 * (py / 10));
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
    SchematicItem *item = NULL;
    qInfo() << event->scenePos();
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
            qInfo() << schemIt->getType();
            if (schemIt->getType() == "node"){
                curWire->setLine(0, 0, it->scenePos().x() - startPos.x(), it->scenePos().y() - startPos.y());
                item = schemIt;
                break;
            }
        }
        if (item == 0) {
            QPointF endPos = gridPos(event->scenePos());
            curWire->setLine(0, 0, endPos.x() - startPos.x(), endPos.y() - startPos.y());
        }
        stopDrawingWire(qgraphicsitem_cast<Node *>(item));
        mode = Schematic::Edit;
        break;

    case Schematic::Edit:
        item = qgraphicsitem_cast<SchematicItem *>(itemAt(event->scenePos(), QTransform()));
        if (!item) break;

        if (item->getType() == "node") {
           startPos = item->scenePos();
           activeNode = qgraphicsitem_cast<Node *>(item);
           startDrawingWire();
           mode = Schematic::Draw;
        } else if (item->getType() == "element") {
            item->setPos(gridPos(event->scenePos()));
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
        curWire->setLine(0, 0, curPos.x() - startPos.x(), curPos.y() - startPos.y());
        break;
    case Schematic::Edit:
        break;
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void Schematic::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    qInfo() << "schematic received drag move event";
    QGraphicsScene::dragMoveEvent(event);
}

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

void Schematic::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete) {
        for (auto it : selectedItems())
            removeItem(it);
    }
    QGraphicsScene::keyReleaseEvent(event);
}
