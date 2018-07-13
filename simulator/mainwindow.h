#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "schematic.h"
#include "elementselector.h"

#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVector>
#include <QMouseEvent>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QGraphicsView *view;
    Schematic *schem; // CentralWidget - handles circuit schematic
    ElementSelector *selector; // left DockWidget - handles element menu
};

#endif // MAINWINDOW_H
