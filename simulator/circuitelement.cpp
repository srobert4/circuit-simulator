#include "circuitelement.h"

CircuitElement::CircuitElement(
        int x, int y,
        int width,
        int height,
        QString imagePath,
        QWidget *parent
) : QWidget(parent)
{
    img = new QLabel(parent);
    QPixmap pm(imagePath);
    img->setPixmap(pm.scaled(width, height));
    img->setGeometry(x, y, width, height);
    img->show();

    nodes.append(QPoint(x - width / 2, y));
    nodes.append(QPoint(x + width / 2, y));
}
