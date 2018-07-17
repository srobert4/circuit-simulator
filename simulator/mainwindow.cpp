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
                        "/home/srobertson/Downloads/resistorSelected.png",
                        "/home/srobertson/Downloads/resistorShadow.png");
    selector->addButton("Capacitor",
                        "/home/srobertson/Downloads/capacitor.png",
                        "/home/srobertson/Downloads/capacitorSelected.png",
                        "/home/srobertson/Downloads/capacitorShadow.png");
    selector->addButton("Inductor",
                        "/home/srobertson/Downloads/inductor.png",
                        "/home/srobertson/Downloads/inductorSelected.png",
                        "/home/srobertson/Downloads/inductorShadow.png");
    selector->addButton("Ground",
                        "/home/srobertson/Downloads/ground.png",
                        "/home/srobertson/Downloads/groundSelected.png",
                        "/home/srobertson/Downloads/groundShadow.png");
    selector->addButton("Pressure",
                        "/home/srobertson/Downloads/voltageSource.png",
                        "/home/srobertson/Downloads/voltageSourceSelected.png",
                        "/home/srobertson/Downloads/voltageSourceShadow.png");
    selector->addButton("Flow",
                        "/home/srobertson/Downloads/currentSource.png",
                        "/home/srobertson/Downloads/currentSourceSelected.png",
                        "/home/srobertson/Downloads/currentSourceShadow.png");


    // Set up schematic graphics view
    view = new QGraphicsView(schem, this);
    view->setMouseTracking(true);
    view->setDragMode(QGraphicsView::RubberBandDrag);
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
