#include "circuitelement.h"

/* Constructor: CircuitElement(int, int, int, int, int, QString, QWidget *)
 * ------------------------------------------------------------------------
 * Constructs a CircuitElement object at position (x,y) and adds the
 * image at the given imagePath and a default label. Creates a dialogBox
 * to set the label values on double click.
 */
CircuitElement::CircuitElement(
        int width, int height, // width/height of image
        int id, int nodeOneID, int nodeTwoID,
        QString imagePath,
        QGraphicsItem *parent
) : QGraphicsItem(parent)
{
    setFlag(ItemHasNoContents); // optimization
    setAcceptHoverEvents(true);
    // Element properties
    this->id = id;
    this->nodeOneId = nodeOneID;
    this->nodeTwoId = nodeTwoID;

    // create Dialog box
    dialogBox = createDialogBox();

    // Initialize label
    name = "element name";
    value = "xx.xx";
    units = "unit";

    // Add children - symbol, nodes, label
    symbol = new Symbol(width, height, imagePath, this);
    connect(symbol, SIGNAL(doubleClicked()),
            this, SLOT (slotSymbolDoubleClicked()));

    nodeOne = new Node(nodeOneId, symbol);
    nodeTwo = new Node(nodeTwoId, symbol);

    label = new QGraphicsSimpleTextItem(name + "\n" + value + units, symbol);

    // Set children positions using child->setPos(pos relative to symbol)
    nodeOne->setPos(-width / 2, 0);
    nodeTwo->setPos(width / 2, 0);
    label->setPos(0, height / 2);
}

// ----------- PUBLIC FUNCTIONS -----------------------------------------------

/* Public Function: boundingRect()
 * -------------------------------
 * Required for QGraphicsItem. Returns
 * null QRectF since contains no items.
 */
QRectF CircuitElement::boundingRect() const
{
    return QRectF();
}

/* Public Function: paint(...)
 * ---------------------------
 * Required for QGraphicsItem.
 * No-op.
 */
void CircuitElement::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
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
}

// ----------- SLOTS -------------------------------------

void CircuitElement::slotSymbolDoubleClicked()
{
    setupDialog();

    int ret = dialogBox->exec();
    if (ret == QDialog::Rejected) return; // TODO reset dialog

    processDialogInput();
}

// ----------- EVENT HANDLERS -------------------------

void CircuitElement::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    qInfo() << "Hovering over circuit element";
    QGraphicsItem::hoverEnterEvent(event);
}
