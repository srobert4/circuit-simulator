#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "schematic.h"
#include "elementselector.h"

#include <QtWidgets>

/* CLASS: MainWindow
 * =================
 * Inherits: QMainWindow
 * Parent: None
 * Children:
 *  - Schematic
 *  - ElementSelector
 *
 * The MainWindow is responsible for setting up the main window with a central
 * widget (Schematic), left dock widget (ElementSelector) and toolbar.
 * The MainWindow adds buttons to the ElementSelector.
 */

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
