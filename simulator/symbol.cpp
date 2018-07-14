#include "symbol.h"

Symbol::Symbol(const QPixmap &image, const QPixmap &selectedImage, int id, QGraphicsItem *parent)
    : SchematicItem(id, "symbol", parent)
{
    setAcceptHoverEvents(true);
    setFlag(ItemIsFocusable);
    setFlag(ItemIsMovable);
    normal = image;
    this->width = normal.width();
    this->height = normal.height();

    selected = selectedImage;

    display = normal;
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
    QGraphicsItem::mousePressEvent(event);
}

void Symbol::focusInEvent(QFocusEvent *event)
{
    display = selected;
    update();
    QGraphicsItem::focusInEvent(event);
}

void Symbol::focusOutEvent(QFocusEvent *event)
{
    display = normal;
    update();
    QGraphicsItem::focusOutEvent(event);
}
