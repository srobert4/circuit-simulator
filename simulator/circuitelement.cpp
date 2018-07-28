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


    nodeOne = new Node(this, this);
    nodeTwo = new Node(this, this);
    nodeOne->setPos(QPointF(-_width / 2, 0));
    nodeTwo->setPos(QPointF(_width / 2, 0));

    dialogBox = nullptr;
    if (!properties.hasLabel) {
        label = nullptr;
        return;
    }

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

// ========= PUBLIC FUNCTIONS ==========================

void CircuitElement::rotate(qreal angle)
{
    // rotate pixmap
    setRotation(rotation() + angle);

    // delete nodes
    QSet<Node *> nodes = nodeOne->getAllNodesSet();
    delete nodeOne;
    nodeOne = new Node(this, this);
    foreach(Node *node, nodes)
        nodeOne->connectNode(node);

    nodes = nodeTwo->getAllNodesSet();
    delete nodeTwo;
    nodeTwo = new Node(this, this);
    foreach(Node *node, nodes)
        nodeTwo->connectNode(node);
    // create new nodes and connect

    nodeOne->setPos(QPointF(-_width / 2, 0));
    nodeOne->setRotation(-rotation());
    nodeTwo->setPos(QPointF(_width / 2, 0));
    nodeTwo->setRotation(-rotation());

    if (!label) return;
    label->setRotation(-rotation());
    // move label to appropriate position
    int rot = static_cast<int>(rotation()) % 360;
    if (rot == 0 || abs(rot) == 360) {
        label->setPos(-label->boundingRect().width() / 2, _height / 2);
    } else if (rot == 90 || rot == -270) {
        label->setPos(-label->boundingRect().width() / 2, -_height / 2);
    } else if (abs(rot) == 180) {
        label->setPos(label->boundingRect().width() / 2, -_height / 2);
    } else if (rot == -90 || rot == 270) {
        label->setPos(label->boundingRect().width() / 2, _height / 2);
    }
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

    QDialogButtonBox *buttonBox = new QDialogButtonBox(dialog);
    QPushButton *cancelButton = new QPushButton("Cancel", dialog);
    QPushButton *doneButton  = new QPushButton("Done", dialog);
    buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);
    buttonBox->addButton(doneButton, QDialogButtonBox::AcceptRole);
    doneButton->setDefault(true);

    QGridLayout *layout = new QGridLayout;

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
        QGridLayout *constValueLayout = new QGridLayout;
        constValueLayout->addWidget(valueLineEdit, 0, 2, 1, 2);
        constValueLayout->addWidget(unitsComboBox, 0, 4);
        constValueLayout->addWidget(unitsLabel, 0, 5);
        constValueExt->setLayout(constValueLayout);

        // --- EXTERNAL VALUE OPTIONS ---
        extValueExt = new QWidget(dialog);
        QWidget *browser = new QWidget;
        QHBoxLayout *browserLayout = new QHBoxLayout;
        valueFileLineEdit = new QLineEdit(dialog);
        QPushButton *browseButton = new QPushButton("Browse", dialog);
        browserLayout->addWidget(valueFileLineEdit);
        browserLayout->addWidget(browseButton);
        browser->setLayout(browserLayout);
        periodLineEdit = new QLineEdit(dialog);
        QFormLayout *extValueLayout = new QFormLayout;

        connect(browseButton, &QPushButton::pressed,
                [=](){ valueFileLineEdit->setText( QFileDialog::getOpenFileName(
                                                        extValueExt,
                                                        "Choose input file",
                                                        "/home/srobertson",
                                                        "All files (*.*)") ); });

        extValueLayout->addRow("Input file: ", browser);
        extValueLayout->addRow("Period: ", periodLineEdit);
        extValueExt->setLayout(extValueLayout);

        // --- CONNECT EXTENSIONS ---
        layout->addWidget(constValueExt, 2, 1, 1, 5);
        layout->addWidget(extValueExt, 2, 1, 1, 5);
        constValueExt->hide();
        extValueExt->hide();

        connect(constButton, &QRadioButton::toggled, [=](){
            constValueExt->setVisible(true);
            extValueExt->setHidden(true);
            doneButton->setEnabled(true);
        });
        connect(externalButton, &QRadioButton::toggled, [=](){
           constValueExt->setHidden(true);
           extValueExt->setVisible(true);
           doneButton->setEnabled(valueFileLineEdit->text() != "" &&
                       QFile::exists(valueFileLineEdit->text()));
        });
        connect(valueFileLineEdit, &QLineEdit::textChanged, [=](){
            bool periodOk;
            periodLineEdit->text().toDouble(&periodOk);
            doneButton->setEnabled(valueFileLineEdit->text() != "" &&
                        QFile::exists(valueFileLineEdit->text()) && periodOk);
        });
        connect(periodLineEdit, &QLineEdit::textChanged, [=](){
            bool periodOk;
            periodLineEdit->text().toDouble(&periodOk);
            doneButton->setEnabled(valueFileLineEdit->text() != "" &&
                        QFile::exists(valueFileLineEdit->text()) && periodOk);
        });
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
    int rot = static_cast<int>(rotation()) % 360;
    if (rot == 0 || abs(rot) == 360) {
        label->setPos(-label->boundingRect().width() / 2, _height / 2);
    } else if (rot == 90 || rot == -270) {
        label->setPos(-label->boundingRect().width() / 2, -_height / 2);
    } else if (abs(rot) == 180) {
        label->setPos(label->boundingRect().width() / 2, -_height / 2);
    } else if (rot == -90 || rot == 270) {
        label->setPos(label->boundingRect().width() / 2, _height / 2);
    }
}


// ========= EVENT HANDLERS ============================================

/* Mouse Event: mouseDoubleClickEvent(...)
 * ---------------------------------------
 * Show dialog box and process input
 */
void CircuitElement::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (!dialogBox) return;
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
