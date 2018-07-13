#ifndef SYMBOL_H
#define SYMBOL_H

#include <QtWidgets>

class Symbol : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit Symbol(
        int width, int height, // image size
        QString &imagePath,
        QGraphicsItem *parent = nullptr
    );

    // required functions
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *) { emit doubleClicked(); }

private:
    QPixmap display;
    QPixmap normal, selected, drag;
    int width, height;

signals:
    void doubleClicked();

public slots:
};

#endif // SYMBOL_H
