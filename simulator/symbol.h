#ifndef SYMBOL_H
#define SYMBOL_H

#include <QtWidgets>
#include "schematicitem.h"

class Symbol : public SchematicItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit Symbol(
        const QPixmap &image,
        const QPixmap &selectedImage,
        int id,
        QGraphicsItem *parent = nullptr
    );

    // required functions
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *) { emit doubleClicked(); }
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

private:
    QPixmap display;
    QPixmap normal, selected, drag;
    int width, height;

signals:
    void doubleClicked();

public slots:
};

#endif // SYMBOL_H
