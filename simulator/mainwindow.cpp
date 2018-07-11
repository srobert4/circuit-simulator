#include "mainwindow.h"


/* Constructor: MainWindow(QWidget *parent)
 * ----------------------------------------
 * MainWindow handles the communication between the
 * schematic (drawing area) and the selector.
 *
 * The MainWindow layout has the Schematic as the CentralWidget
 * and the Selector is set as the left dockWidget.
 *
 * The MainWindow is currently responsible for adding buttons
 * to the Selector.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QGraphicsView view = new QGraphicsView(this);
    // Create Schematic and Selector
    schem = new Schematic(this);
    selector = new ElementSelector(this);
    selector->addButton("Resistor",
                        "/home/srobertson/Downloads/res.png");
    selector->addButton("Capacitor",
                        "/home/srobertson/Downloads/cap.png");

    // Set up layout
    view.setScene(schem);
    setCentralWidget(view);
    QDockWidget *dockSelector = new QDockWidget;
    dockSelector->setWidget(selector);
    dockSelector->setAllowedAreas(Qt::LeftDockWidgetArea);
    addDockWidget(
                Qt::LeftDockWidgetArea,
                dockSelector);


}

MainWindow::~MainWindow()
{

}

/*
 * Slot: slotSchematicClicked()
 * ----------------------------
 * This slot is called when the mouse is
 * clicked on the Schematic. The MainWindow asks
 * the Selector if there is an element selected, and
 * if there is adds this element to the Schematic.
 */
void MainWindow::slotSchematicClicked()
{
    QString path = selector->getElementPath();
    if (path == "") return;
    schem->addElement(path);
}
