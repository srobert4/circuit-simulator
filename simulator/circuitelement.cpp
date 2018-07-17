#include "circuitelement.h"

/* Constructor: CircuitElement(const QPixmap, const QPixmap, QGraphicsItem*)
 * ------------------------------------------------------------------------
 * Create and initialize symbol, label and dialog box
 */
CircuitElement::CircuitElement(
        const QPixmap image,
        const QPixmap selectedImage,
        QGraphicsItem *parent
) : SchematicItem("element", parent) // TODO later change type to arg and name by element type e.g. resistor
{
    setFlag(ItemIsSelectable, true);
    setFlag(ItemIsFocusable, true);
    setFlag(ItemIsMovable, true);
    setAcceptHoverEvents(true);

    // Create symbol
    normal = image;
    this->_width = normal.width();
    this->_height = normal.height();

    selected = selectedImage;
    display = selected; // icon is selected when placed
    setSelected(true);

    // create Dialog box
    dialogBox = createDialogBox();

    // Initialize label
    name = "element name";
    value = "xx.xx";
    units = "--";

    // Label doesn't inherit SchematicItem so set data ourselves
    label = new QGraphicsSimpleTextItem(name + "\n" + value + units, this);
    label->setData(TypeKey, "label");

    // Set children positions using child->setPos(pos relative to symbol)
    label->setPos(-label->boundingRect().width() / 2, image.height() / 2);
}

// ========= PUBLIC FUNCTIONS ================================

/* Public Function: boundingRect()
 * -------------------------------
 * Bounding rect is bounding rect of image
 */
QRectF CircuitElement::boundingRect() const
{
    return QRectF(-_width / 2, -_height / 2, _width, _height);
}

/* Public Function: paint(...)
 * ---------------------------
 * Render symbol pixmap - either selected or normal
 */
void CircuitElement::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    display = (isSelected() ? selected : normal);
    painter->drawPixmap(-_width/2, -_height/2, display);
}

/* Public Function: setNodes(Node*, Node*)
 * ---------------------------------------
 * Provide pointers to the Node objects associated
 * with this element.
 */
void CircuitElement::setNodes(Node *nodeOne, Node *nodeTwo)
{
    this->nodeOne = nodeOne;
    this->nodeTwo = nodeTwo;
}

// ========= PRIVATE FUNCTIONS =========================

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
    unitsComboBox->addItems({"--", "Ohms", "Farads"});

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


// ========= EVENT HANDLERS ============================================

/* Mouse Event: mouseDoubleClickEvent(...)
 * ---------------------------------------
 * Show dialog box and process input
 */
void CircuitElement::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    setupDialog();

    int ret = dialogBox->exec();
    if (ret == QDialog::Rejected) return;

    processDialogInput();

    QGraphicsItem::mouseDoubleClickEvent(event);
}

/* Hover Event: hoverEnterEvent(...)
 * ---------------------------------
 * Show associated nodes
 */
void CircuitElement::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if(!(nodeOne && nodeTwo)) return;
    nodeOne->showNode();
    nodeTwo->showNode();
    QGraphicsItem::hoverEnterEvent(event);
}

/* Hover Event: hoverMoveEvent(...)
 * --------------------------------------------------------
 * Show associated nodes. This function must be implemented
 * in order for nodes to remain shown if mouse hovers from node
 * to element without exiting element boundingRect.
 */
void CircuitElement::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if(!(nodeOne && nodeTwo)) return;
    nodeOne->showNode();
    nodeTwo->showNode();
    QGraphicsItem::hoverMoveEvent(event);
}

/* Hover Event: hoverLeaveEvent(...)
 * ---------------------------------
 * Hide associated nodes
 */
void CircuitElement::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if(!(nodeOne && nodeTwo)) return;
    nodeOne->hideNode();
    nodeTwo->hideNode();
    QGraphicsItem::hoverLeaveEvent(event);
}

/* Focus Event: focusInEvent(...)
 * ------------------------------
 * Display selected image
 */
void CircuitElement::focusInEvent(QFocusEvent *event)
{
    display = selected;
    update();
    QGraphicsItem::focusInEvent(event);
}

/* Focus Event: focusOutEvent(...)
 * -------------------------------
 * Display normal image
 */
void CircuitElement::focusOutEvent(QFocusEvent *event)
{
    display = normal;
    update();
    QGraphicsItem::focusOutEvent(event);
}
