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
    explicit Schematic(QWidget *parent = nullptr);
    void addElement(QString path);
    bool isDrawing() {return drawing;}

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *) override;

signals:
    void schematicClicked();

public slots:
    void slotNodeClicked(QPoint clickPos, int nodeId);

private:
    // Track components
    QMap<int, CircuitElement*> elements;
    QMap<int, Node*> nodes;
    QMap<int, Wire*> wires;
    int elemId, nodeId, wireId;

    // Needed for drawing wires
    int x_last_click, y_last_click;
    int activeNode;
    bool drawing;
    QPoint start_pos;
    QPoint cur_pos;

    // Drawing functions
    void startDrawingWire(QPoint start, int nodeId);
    void stopDrawingWire(QPoint end, int nodeId);
    void addWire(QPoint end, int endNode);
};

#endif // SCHEMATIC_H
