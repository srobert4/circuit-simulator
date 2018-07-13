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
    // Create Schematic and Selector
    schem = new Schematic(this);

    selector = new ElementSelector(schem, this);
    selector->addButton("Resistor",
                        "/home/srobertson/Downloads/resistor.png",
                        "/home/srobertson/Downloads/resistorShadow.png");
    selector->addButton("Capacitor",
                        "/home/srobertson/Downloads/cap.png",
                        "/home/srobertson/Downloads/cap.png");

    // Set up schematic graphics view
    view = new QGraphicsView(schem, this);
    view->setMouseTracking(true);
    schem->setSceneRect(0, 0, 800, 800); // set initial size
    setCentralWidget(view);

    // Set up selector dock
    QDockWidget *dockSelector = new QDockWidget;
    dockSelector->setWidget(selector);
    dockSelector->setAllowedAreas(Qt::LeftDockWidgetArea);
    addDockWidget(
        Qt::LeftDockWidgetArea,
        dockSelector
    );
}

MainWindow::~MainWindow()
{

}
