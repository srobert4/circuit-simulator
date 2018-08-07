#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "schematic.h"
#include "elementselector.h"

#include <QtWidgets>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private:
    QGraphicsView *view;
    Schematic *schem; // CentralWidget - handles circuit schematic
    ElementSelector *selector; // left DockWidget - handles element menu
};

#endif // MAINWINDOW_H
