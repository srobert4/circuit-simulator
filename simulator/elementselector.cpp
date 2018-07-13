#include "elementselector.h"

/* Constructor: ElementSelector(QWidget *parent)
 * ---------------------------------------------
 * The constructor creates a QButtonGroup to hold
 * the buttons, a QGridLayout and connects the signal
 * emitted when the Selector is clicked outside a button
 * to the slot to deselect all buttons.
 */
ElementSelector::ElementSelector(Schematic *schematic, QWidget *parent
) : QWidget(parent)
{
    this->schematic = schematic;
    connect(schematic, SIGNAL(schematicClicked()),
            this, SLOT(slotSchematicClicked()));

    buttons = new QButtonGroup(this);
    connect(buttons, SIGNAL(buttonPressed(int)),
            this, SLOT(slotButtonPressed(int)));
    connect(buttons, SIGNAL(buttonReleased(int)),
            this, SLOT(slotButtonReleased(int)));
    deselectOnRelease = false;

    layout = new QGridLayout;
    setLayout(layout);
}

// ----------- PUBLIC FUNCTIONS -----------------------------------------------

/* Public Function: addButton(const QString &, const QString &)
 * ------------------------------------------------------------
 * addButton creates a QPushButton and adds it to the
 * QButtonGroup. The buttons are currently set to be checkable
 * but this should be changed. The button is given a unique
 * integer ID which is used to associate buttons with path names
 * and element types.
 */
void ElementSelector::addButton(
    const QString &elemType,
    const QString &imgPath,
    const QString &imgShadowPath
)
{
    // create button
    QPushButton *button = new QPushButton(elemType, this);
    button->setCheckable(true);

    // add button to selector
    buttons->addButton(button, curId);
    layout->addWidget(button, curId, 0);

    // associate with path and element
    imagePaths[curId] = imgPath;
    shadowImagePaths[curId] = imgShadowPath;
    elementTypes[curId] = elemType;    
    curId++;
}

/* Public Function: getElementName()
 * ---------------------------------
 * Returns the element type of the currently checked
 * button or the empty string if no button is checked.
 */
QString ElementSelector::getElementName()
{
    int checked = buttons->checkedId();
    if (checked == -1)
        return "";
    return elementTypes[checked];
}

/* Public Function: getElementPath()
 * ---------------------------------
 * Returns the image path of the currently checked
 * button or the empty string if no button is checked.
 */
QString ElementSelector::getElementPath()
{
    int checked = buttons->checkedId();
    if (checked == -1)
        return "";
    return imagePaths[checked];
}
// ----------- PRIVATE FUNCTIONS ------------------------------

/* Slot: deselectAll()
 * --------------------
 * Deselects all buttons
 */
void ElementSelector::deselectAll()
{
    buttons->setExclusive(false);
    QAbstractButton *checked = buttons->checkedButton();
    if (checked == NULL) return;
    checked->setChecked(false);
    buttons->setExclusive(true);
}

// ----------- SLOTS -----------------------------------------------

/* Slot: slotButtonPressed(int)
 * ----------------------------
 * Check if button pressed is already selected,
 * in which case set deselectOnRelease so that
 * it is deselected when mouse is released.
 */
void ElementSelector::slotButtonPressed(int id)
{
    if (id == buttons->checkedId())
        deselectOnRelease = true;
}


/* Slot: slotButtonReleased(int)
 * -------------------------
 * Deselect all if checked button
 * is clicked.
 */
void ElementSelector::slotButtonReleased(int id)
{
    if (deselectOnRelease) {
        deselectAll();
        deselectOnRelease = false;
        return;
    }
    schematic->setMode(Schematic::Build);
    schematic->setImagePaths(imagePaths[id], shadowImagePaths[id]);
}
