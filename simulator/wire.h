#ifndef WIRE_H
#define WIRE_H

#include <QObject>
#include <QWidget>

class Wire : public QWidget
{
    Q_OBJECT
public:
    explicit Wire(QPoint start, QPoint end, int id, QWidget *parent = nullptr);
    void setStartNode(int node) { node1 = node; }
    void setEndNode(int node) { node2 = node; }

protected:
    void paintEvent(QPaintEvent *) override;

private:
    QPoint start;
    QPoint end;
    int id, node1, node2;

signals:

public slots:
};

#endif // WIRE_H
