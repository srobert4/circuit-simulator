#include "symbol.h"

Symbol::Symbol(int width, int height, QString &imagePath, QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    setAcceptHoverEvents(true);
    pixmap = QPixmap(imagePath);
    pixmap = pixmap.scaled(width, height);
    this->width = pixmap.width();
    this->height = pixmap.height();

}

QRectF Symbol::boundingRect() const
{
    return QRectF(-width / 2, -height / 2, width, height);
}

void Symbol::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->drawPixmap(-width / 2, -height / 2, pixmap);
}

void Symbol::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    qInfo() << "Hovering over a symbol";
    QGraphicsItem::hoverEnterEvent(event);
}
