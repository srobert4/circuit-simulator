#ifndef NODE_H
#define NODE_H

#include <QtWidgets>

class Node : public QWidget
{
    Q_OBJECT
public:
    explicit Node(int x, int y, int id, QWidget *parent = nullptr);
    void setElemId(int elem) { elemId = elem; }
    void addWire(int wire) { wires.append(wire); }

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
    int id, elemId;
    QVector<int> wires;

signals:
    void nodeClicked(QPoint center, int id);
};

#endif // NODE_H
