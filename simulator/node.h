#ifndef NODE_H
#define NODE_H

#include <QtWidgets>

class Node : public QWidget
{
    Q_OBJECT
public:
    explicit Node(int x, int y, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *) override;

private:
    QEvent::Type last_event;
    QPoint center;
    QPoint globalCenter;
    int rad;

signals:
    void nodeClicked(QPoint center);
};

#endif // NODE_H
