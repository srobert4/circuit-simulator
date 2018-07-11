#include "circuitelement.h"

/* Constructor: CircuitElement(int, int, int, int, int, QString, QWidget *)
 * ------------------------------------------------------------------------
 * Constructs a CircuitElement object at position (x,y) and adds the
 * image at the given imagePath and a default label. Creates a dialogBox
 * to set the label values on double click.
 */
CircuitElement::CircuitElement(
        int x, int y, // top left position of QWidget
        int width, int height, // width/height of image
        int id,
        QString imagePath,
        QWidget *parent
) : QWidget(parent)
{
    // QWidget properties
    move(x, y);
    this->id = id;

    // create Dialog box
    dialogBox = createDialogBox();

    // Initialize label
    name = "element name";
    value = "xx.xx";
    units = "unit";

    // Create image and label
    img = new QLabel(this);
    QPixmap pm(imagePath);
    img->setPixmap(pm.scaled(width, height));
    nameLabel = new QLabel(name, this);
    valueLabel = new QLabel(value + units, this);

    // set layout
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(img, 0, 0, Qt::AlignTop | Qt::AlignLeft);
    layout->addWidget(nameLabel, 1, 0, Qt::AlignCenter);
    layout->addWidget(valueLabel, 2, 0, Qt::AlignCenter);

    setLayout(layout);
    show();
}

// ----------- PUBLIC FUNCTIONS -----------------------------------------------

/* Public Function: setNodeIds(int, int)
 * -------------------------------------
 * Associates the element nodes with the
 * IDs tracked by the Schematic.
 */
void CircuitElement::setNodeIds(int node1, int node2)
{
    this->node1 = node1;
    this->node2 = node2;
}

/* Public Function: getNodePosition(QPoint&, QPoint&)
 * --------------------------------------------------
 * Sets the value of the arguments to the position of
 * the left and right nodes of the element (ends of the
 * image)
 */
void CircuitElement::getNodePosition(QPoint &left, QPoint &right)
{
    QPoint pos = this->pos() + img->pos();
    left = pos + QPoint(0, img->height() / 2);
    right = left + QPoint(img->width(), 0);
}

// ----------- PRIVATE FUNCTIONS -----------------------------------------------

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
    units = unitsComboBox->currentText();

    nameLabel->setText(name);
    valueLabel->setText(value + units);
}

// ----------- EVENT HANDLERS -------------------------------------

/* mousePress: for debugging */
void CircuitElement::mousePressEvent(QMouseEvent *)
{
    qInfo() << "Element pressed";
}

/* mouseRelease: for debugging */
void CircuitElement::mouseReleaseEvent(QMouseEvent *)
{
    qInfo() << "Element released";
}

/* MouseEvent: mouseDoubleClickEvent(QMouseEvent*)
 * -----------------------------------------------
 * Opens dialog box for user to input element params
 */
void CircuitElement::mouseDoubleClickEvent(QMouseEvent *)
{
    setupDialog();

    int ret = dialogBox->exec();
    if (ret == QDialog::Rejected) return; // TODO reset dialog

    processDialogInput();
}
