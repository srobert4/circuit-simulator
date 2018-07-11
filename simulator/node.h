#ifndef NODE_H
#define NODE_H

#include <QtWidgets>

class Node : public QWidget
{
    Q_OBJECT
public:
    explicit Node(
        int x, int y, // center point
        int id,
        QWidget *parent = nullptr
    );
    void setElemId(int elem) { elemId = elem; }
    void addWire(int wire) { wires.append(wire); }

protected:
    void paintEvent(QPaintEvent *) override; // -> draw node
    void enterEvent(QEvent *event) override; // -> show node
    void leaveEvent(QEvent *event) override; // -> clear node
    void mousePressEvent(QMouseEvent *) override; // -> nodeClicked()

private:
    QEvent::Type last_event;
    QPoint center; // center relative to self
    QPoint globalCenter; // center relative to Schematic
    int rad;
    int id, elemId; // track self and associated element
    QVector<int> wires; // track wires beginning or ending at this node

signals:
    void nodeClicked(QPoint center, int id); // -> (Schematic) slotNodeClicked()
};

#endif // NODE_H
