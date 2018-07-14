#include "circuitelement.h"

/* Constructor: CircuitElement(int, int, int, int, int, QString, QWidget *)
 * ------------------------------------------------------------------------
 * Constructs a CircuitElement object at position (x,y) and adds the
 * image at the given imagePath and a default label. Creates a dialogBox
 * to set the label values on double click.
 */
CircuitElement::CircuitElement(// width/height of image
        int id, int nodeOneID, int nodeTwoID,
        const QPixmap image,
        const QPixmap selectedImage,
        QGraphicsItem *parent
) : SchematicItem(id, "element", parent) // later change type to arg and name by element type e.g. resistor
{
    setFlag(ItemIsSelectable, true);
    setFlag(ItemIsFocusable, true);
    setFlag(ItemIsMovable, true);
    setAcceptHoverEvents(true);

    // Create symbol
    normal = image;
    this->width = normal.width();
    this->height = normal.height();

    selected = selectedImage;
    display = selected; // icon is selected when placed
    setSelected(true);

    // create Dialog box
    dialogBox = createDialogBox();

    // Initialize label
    name = "element name";
    value = "xx.xx";
    units = "unit";

    // Add Nodes
    this->nodeOneId = nodeOneID;
    this->nodeTwoId = nodeTwoID;
    nodeOne = new Node(nodeOneId, id, this);
    nodeTwo = new Node(nodeTwoId, id, this);

    // Label doesn't inherit SchematicItem so set data ourselves
    label = new QGraphicsSimpleTextItem(name + "\n" + value + units, this);
    label->setData(TypeKey, "label");
    label->setData(IDKey, id);

    // Set children positions using child->setPos(pos relative to symbol)
    nodeOne->setPos(-image.width() / 2, 0);
    nodeTwo->setPos(image.width() / 2, 0);
    label->setPos(-label->boundingRect().width() / 2, image.height() / 2);
}

// ----------- PUBLIC FUNCTIONS -----------------------------------------------

/* Public Function: boundingRect()
 * -------------------------------
 * Required for QGraphicsItem. Returns
 * null QRectF since contains no items.
 */
QRectF CircuitElement::boundingRect() const
{
    return QRectF(-width / 2, -height / 2, width, height); // TODO - include text
}

/* Public Function: paint(...)
 * ---------------------------
 * Render symbol pixmap
 */
void CircuitElement::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    display = (isSelected() ? selected : normal);
    painter->drawPixmap(-width/2, -height/2, display);
}

/* Public Function: setNodeIds(int, int)
 * -------------------------------------
 * Associates the element nodes with the
 * IDs tracked by the Schematic.
 */
void CircuitElement::setNodeIds(int nodeOne, int nodeTwo)
{
    this->nodeOneId = nodeOne;
    this->nodeTwoId = nodeTwo;
}

/* Public Function: getNodeIds(int&, int&)
 * -------------------------------------
 * Sets arguments to node IDS for two nodes
 */
void CircuitElement::getNodeIds(int &nodeOne, int &nodeTwo)
{
    nodeOne = this->nodeOneId;
    nodeTwo = this->nodeTwoId;
}

// ----------- PRIVATE FUNCTIONS --------------------------

/* Private Function: createDialogBox()
 * -----------------------------------
 * Create the dialog box that pops up
 * when the element is double clicked.
 */
QDialog *CircuitElement::createDialogBox()
{
    QDialog *dialog = new QDialog();
    dialog->setModal(true);

    QLabel *nameLabel = new QLabel("Element Name: ");
    nameLineEdit = new QLineEdit;

    QLabel *valueLabel = new QLabel("Value: ");
    valueLineEdit = new QLineEdit;

    QLabel *unitsLabel = new QLabel("Units: ");
    unitsComboBox = new QComboBox;
    unitsComboBox->addItems({"Ohms", "Farads"});

    QPushButton *cancelButton = new QPushButton("Cancel");
    QPushButton *doneButton  = new QPushButton("Done");
    doneButton->setDefault(true);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(nameLabel, 0, 0);
    layout->addWidget(nameLineEdit, 0, 1, 1, 2);
    layout->addWidget(valueLabel, 1, 0);
    layout->addWidget(valueLineEdit, 1, 1, 1, 2);
    layout->addWidget(unitsLabel, 2, 0);
    layout->addWidget(unitsComboBox, 2, 1, 1, 2);
    layout->addWidget(cancelButton, 3, 1);
    layout->addWidget(doneButton, 3, 2);
    dialog->setLayout(layout);

    QApplication::connect(cancelButton, SIGNAL(clicked(bool)),
            dialog, SLOT(reject()));
    QApplication::connect(doneButton, SIGNAL(clicked(bool)),
            dialog, SLOT(accept()));

    return dialog;
}

/* Private Function: setupDialog()
 * -------------------------------
 * Called before the dialogBox is displayed.
 * Fills the dialogBox prompts with the current
 * label values.
 */
void CircuitElement::setupDialog()
{
    nameLineEdit->setText(name);
    valueLineEdit->setText(value);
    unitsComboBox->setCurrentText(units);
}

/* Private Function: processDialogInput()
 * --------------------------------------
 * Called after the dialogBox is closed if
 * the return value is QDialog::Accepted.
 * Updates label-related instance variables
 * with input in dialogBox.
 */
void CircuitElement::processDialogInput()
{
    name = nameLineEdit->text();
    value = valueLineEdit->text();
    units = unitsComboBox->currentText();

    label->setText(name + "\n" + value + units);
    label->setPos(-label->boundingRect().width() / 2, this->boundingRect().height() / 2);
}


// ----------- EVENT HANDLERS -------------------------

void CircuitElement::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    setupDialog();

    int ret = dialogBox->exec();
    if (ret == QDialog::Rejected) return; // TODO reset dialog

    processDialogInput();

    QGraphicsItem::mouseDoubleClickEvent(event);
}

void CircuitElement::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    nodeOne->showNode();
    nodeTwo->showNode();
    QGraphicsItem::hoverEnterEvent(event);
}

void CircuitElement::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    nodeOne->hideNode();
    nodeTwo->hideNode();
    QGraphicsItem::hoverLeaveEvent(event);
}

void CircuitElement::focusInEvent(QFocusEvent *event)
{
    display = selected;
    update();
    QGraphicsItem::focusInEvent(event);
}

void CircuitElement::focusOutEvent(QFocusEvent *event)
{
    display = normal;
    update();
    QGraphicsItem::focusOutEvent(event);
}
