#ifndef SCHEMATIC_H
#define SCHEMATIC_H

#include <QtWidgets>

#include "circuitelement.h"
#include "node.h"
#include "wire.h"

class Schematic : public QWidget
{
    Q_OBJECT
public:
    explicit Schematic(
            int x,
            int y,
            int width,
            int height,
            QWidget *parent = nullptr);
    void addElement(QString path);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *) override;

signals:
    void schematicClicked();

public slots:
    void startDrawingWire(QPoint start);

private:
    QVector<CircuitElement*> *elements;
    QVector<Node*> *nodes;
    QVector<Wire*> *wires;
    int x_last_click;
    int y_last_click;
    bool drawing;
    QPoint start_pos;
    QPoint cur_pos;
};

#endif // SCHEMATIC_H
