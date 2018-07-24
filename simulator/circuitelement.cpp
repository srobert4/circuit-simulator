#include "circuitelement.h"

/* Constructor: CircuitElement(const QPixmap, const QPixmap, QGraphicsItem*)
 * ------------------------------------------------------------------------
 * Create and initialize symbol, label and dialog box
 */
CircuitElement::CircuitElement(
        const ElementProperties properties,
        QGraphicsItem *parent
) : SchematicItem("element", parent, properties.subtype)
{
    setFlag(ItemIsSelectable, true);
    setFlag(ItemIsFocusable, true);
    setFlag(ItemIsMovable, true);
    setAcceptHoverEvents(true);

    // Create symbol
    normal = properties.image;
    this->_width = normal.width();
    this->_height = normal.height();

    selected = properties.selected;
    display = selected; // icon is selected when placed
    setSelected(true);

    dialogBox = nullptr;
    if (!properties.hasLabel) return;

    // create Dialog box
    QString mu;
    const QChar MathSymbolSigma(0x03BC);
    mu.setUnicode(&MathSymbolSigma, 1);
    unitModifiers = { "", "T", "G", "M", "K", "m", mu, "n", "p", "f" };
    dialogBox = createDialogBox(properties.prefix,
                                properties.units,
                                properties.allowsExternalInput);

    // Initialize label
    prefix = properties.prefix;
    name = "";
    value = "";
    unitMod = "";
    units = properties.units;

    // Label doesn't inherit SchematicItem so set data ourselves
    label = new QGraphicsSimpleTextItem(prefix + name + "\n" + value + units, this);
    label->setData(TypeKey, "label");

    // Set children positions using child->setPos(pos relative to symbol)
    label->setPos(-label->boundingRect().width() / 2, _height / 2);
}

CircuitElement::~CircuitElement() { if (dialogBox) delete dialogBox; }

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

Node *CircuitElement::getOtherNode(Node *node)
{
    if (node == nodeOne) return nodeTwo;
    if (node == nodeTwo) return nodeOne;
    return nullptr;
}

// ========= PRIVATE FUNCTIONS =========================

/* Private Function: createDialogBox()
 * -----------------------------------
 * Create the dialog box that pops up
 * when the element is double clicked.
 */
QDialog *CircuitElement::createDialogBox(QString prefix,
                                         QString units,
                                         bool allowsExternalInput)
{
    QDialog *dialog = new QDialog();

    QLabel *nameLabel = new QLabel("Name: ", dialog);
    QLabel *prefixLabel = new QLabel(prefix, dialog);
    nameLineEdit = new QLineEdit(dialog);

    QLabel *valueLabel = new QLabel("Value: ", dialog);
    valueLineEdit = new QLineEdit(dialog);
    unitsComboBox = new QComboBox(dialog);
    unitsComboBox->addItems(unitModifiers);
    QLabel *unitsLabel = new QLabel(units, dialog);

    QPushButton *cancelButton = new QPushButton("Cancel", dialog);
    QPushButton *doneButton  = new QPushButton("Done", dialog);
    doneButton->setDefault(true);

    QGridLayout *layout = new QGridLayout(dialog);

    layout->addWidget(nameLabel, 0, 0);
    layout->addWidget(prefixLabel, 0, 1, Qt::AlignRight);
    layout->addWidget(nameLineEdit, 0, 2, 1, 4);

    layout->addWidget(valueLabel, 1, 0);

    if (allowsExternalInput) {
        // --- TOGGLE EXTENSIONS BUTTONS ---
        QRadioButton *constButton = new QRadioButton("Constant value", dialog);
        QRadioButton *externalButton = new QRadioButton("External value", dialog);
        QButtonGroup *valueTypeButtons = new QButtonGroup(dialog);
        valueTypeButtons->addButton(constButton);
        valueTypeButtons->addButton(externalButton);
        valueTypeButtons->setExclusive(true);

        layout->addWidget(constButton, 1, 2);
        layout->addWidget(externalButton, 1, 3);

        // --- CONST VALUE OPTIONS ---
        constValueExt = new QWidget(dialog);
        QGridLayout *constValueLayout = new QGridLayout(dialog);
        constValueLayout->addWidget(valueLineEdit, 0, 2, 1, 2);
        constValueLayout->addWidget(unitsComboBox, 0, 4);
        constValueLayout->addWidget(unitsLabel, 0, 5);
        constValueExt->setLayout(constValueLayout);

        // --- EXTERNAL VALUE OPTIONS ---
        extValueExt = new QWidget(dialog);
        QLabel *browserLabel = new QLabel("File containing values: ", dialog);
        valueFileLineEdit = new QLineEdit(dialog);
        QPushButton *browseButton = new QPushButton("Browse", dialog);
        QHBoxLayout *extValueLayout = new QHBoxLayout(dialog);

        connect(browseButton, &QPushButton::pressed,
                [=](){ valueFileLineEdit->setText( QFileDialog::getOpenFileName(
                                                        extValueExt,
                                                        "Choose input file",
                                                        "/home/srobertson",
                                                        "All files (*.*)") ); });

        extValueLayout->addWidget(browserLabel);
        extValueLayout->addWidget(valueFileLineEdit);
        extValueLayout->addWidget(browseButton);
        extValueExt->setLayout(extValueLayout);

        // --- CONNECT EXTENSIONS ---
        layout->addWidget(constValueExt, 2, 1, 1, 5);
        layout->addWidget(extValueExt, 2, 1, 1, 5);
        constValueExt->hide();
        extValueExt->hide();

        connect(constButton, &QRadioButton::toggled, constValueExt, &QWidget::setVisible);
        connect(constButton, &QRadioButton::toggled, extValueExt, &QWidget::setHidden);
        connect(externalButton, &QRadioButton::toggled, constValueExt, &QWidget::setHidden);
        connect(externalButton, &QRadioButton::toggled, extValueExt, &QWidget::setVisible);
    } else {
        constValueExt = nullptr;
        extValueExt = nullptr;
        valueFileLineEdit = nullptr;

        layout->addWidget(valueLineEdit, 1, 2, 1, 2);
        layout->addWidget(unitsComboBox, 1, 4);
        layout->addWidget(unitsLabel, 1, 5);
    }

    layout->addWidget(cancelButton, 3, 3);
    layout->addWidget(doneButton, 3, 4);

    dialog->setLayout(layout);

   connect(cancelButton, SIGNAL(clicked(bool)),
            dialog, SLOT(reject()));
    connect(doneButton, SIGNAL(clicked(bool)),
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
    unitMod = unitsComboBox->currentText();
    if (valueFileLineEdit && valueFileLineEdit->text() != "") {
        value = "External";
        label->setText(prefix + name + "\n" + value);
        externalFile = valueFileLineEdit->text();
    } else {
        label->setText(prefix + name + "\n" + value + unitMod + units);
    }
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
