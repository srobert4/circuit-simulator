#include "circuitelement.h"

CircuitElement::CircuitElement(
        QPoint *pos,
        int height,
        int width,
        QString imagePath,
        QWidget *parent
) : QWidget(parent)
{
    img = new QLabel(parent);
    QPixmap pm(imagePath);
    img->setPixmap(pm.scaled(width, height));
    img->setGeometry(pos->x(), pos->y(), width, height);
    img->show();

    nodes.append(*pos + QPoint(0, height / 2));
    nodes.append(*pos + QPoint(width, height / 2));
}
