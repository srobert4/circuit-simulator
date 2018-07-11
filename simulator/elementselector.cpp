#include "elementselector.h"

ElementSelector::ElementSelector(QWidget *parent
) : QWidget(parent)
{
    buttons = new QButtonGroup(this);
    connect(this, SIGNAL(clickedAway()),
                this, SLOT(slotDeselect()));

    layout = new QGridLayout;
    setLayout(layout);
}

void ElementSelector::addButton(
        const QString &elemType,
        const QString &imgPath)
{
    QPushButton *button = new QPushButton(elemType, this);
    button->setCheckable(true);

    buttons->addButton(button, curId);
    imagePaths[curId] = imgPath;
    elementTypes[curId] = elemType;    
    layout->addWidget(button, curId, 0);
    curId++;
}

QString ElementSelector::getElementName()
{
    int checked = buttons->checkedId();
    if (checked == -1)
        return "";
    return elementTypes[checked];
}

QString ElementSelector::getElementPath()
{
    int checked = buttons->checkedId();
    if (checked == -1)
        return "";
    return imagePaths[checked];
}

void ElementSelector::mousePressEvent(QMouseEvent *event)
{
    emit clickedAway();
}

void ElementSelector::slotDeselect()
{
    buttons->setExclusive(false);
    QAbstractButton *checked = buttons->checkedButton();
    if (checked == NULL) return;
    checked->setChecked(false);
    buttons->setExclusive(true);
}
