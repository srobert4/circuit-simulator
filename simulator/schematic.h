#ifndef SCHEMATIC_H
#define SCHEMATIC_H

#include <QWidget>
#include <QVector>
#include <QMouseEvent>
#include <QString>

#include "circuitelement.h"
#include "node.h"

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

signals:
    void schematicClicked();

public slots:

private:
    QVector<CircuitElement*> *elements;
    QVector<Node*> *nodes;
    int x_last_click;
    int y_last_click;
};

#endif // SCHEMATIC_H
