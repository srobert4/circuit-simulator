#include "symbol.h"

Symbol::Symbol(int width, int height, QString &imagePath, QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    setAcceptHoverEvents(true);
    display = QPixmap(imagePath);
    display = display.scaled(width, height, Qt::KeepAspectRatio);
    this->width = display.width();
    this->height = display.height();

}

QRectF Symbol::boundingRect() const
{
    return QRectF(-width / 2, -height / 2, width, height);
}

void Symbol::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->drawPixmap(-width / 2, -height / 2, display);
}

void Symbol::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qInfo() << "Mouse press received by Symbol";
    QGraphicsItem::mousePressEvent(event);
}
