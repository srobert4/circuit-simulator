#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QtWidgets>

class Viewport : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Viewport(QWidget *parent = 0) : QGraphicsView(parent) { setDragMode(QGraphicsView::RubberBandDrag); }
\
protected:
    void resizeEvent(QResizeEvent *event) override {
        this->scene()->setSceneRect(rect());
        resizeEvent(event);
    }

};

#endif // VIEWPORT_H
