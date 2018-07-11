#include "elementselector.h"

/* Constructor: ElementSelector(QWidget *parent)
 * ---------------------------------------------
 * The constructor creates a QButtonGroup to hold
 * the buttons, a QGridLayout and connects the signal
 * emitted when the Selector is clicked outside a button
 * to the slot to deselect all buttons.
 */
ElementSelector::ElementSelector(QWidget *parent
) : QWidget(parent)
{
    buttons = new QButtonGroup(this);
    connect(this, SIGNAL(clickedAway()),
                this, SLOT(slotDeselect()));

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
    const QString &imgPath
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

// ----------- SLOTS -----------------------------------------------

/* Slot: slotDeselect()
 * --------------------
 * Deselects all buttons
 */
void ElementSelector::slotDeselect()
{
    buttons->setExclusive(false);
    QAbstractButton *checked = buttons->checkedButton();
    if (checked == NULL) return;
    checked->setChecked(false);
    buttons->setExclusive(true);
}

// ----------- EVENT HANDLERS -----------------------------------------------

/* MouseEvent: mousePressEvent(QMouseEvent *)
 * ------------------------------------------
 * When the Selector is clicked it emits the
 * clickedAway() signal, which tells the Selector
 * to deselect all buttons.
 */
void ElementSelector::mousePressEvent(QMouseEvent *)
{
    emit clickedAway();
}
