#include "elementselector.h"

ElementSelector::ElementSelector(
        int x,
        int y,
        int width,
        int height,
        QWidget *parent
) : QWidget(parent)
{
    setGeometry(x, y, width, height);
    buttons = new QButtonGroup(this);
    connect(this, SIGNAL(clicked()),
                this, SIGNAL(slotDeselect()));
}

void ElementSelector::addButton(
        const QString &elemType,
        const QString &imgPath,
        int x, int y, int width, int height)
{
    QPushButton *button = new QPushButton(elemType, this);
    button->setGeometry(x, y, width, height);
    button->setCheckable(true);

    buttons->addButton(button, curId);
    imagePaths[curId] = imgPath;
    elementTypes[curId] = elemType;
    curId++;
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

    QAbstractButton *checked = buttons->checkedButton();
    checked->setChecked(false);
}
