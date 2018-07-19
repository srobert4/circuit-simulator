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
    QString units;
    const QChar Ohms(0x03A9);
    units.setUnicode(&Ohms, 1);
    selector->addButton("Resistor",
                        ":/rcs/images/resistor.png",
                        ":/rcs/images/resistorSelected.png",
                        ":/rcs/images/resistorShadow.png",
                        true, false, "R", units, 1);
    selector->addButton("Capacitor",
                        ":/rcs/images/capacitor.png",
                        ":/rcs/images/capacitorSelected.png",
                        ":/rcs/images/capacitorShadow.png",
                        true, false, "C", "F", 1);
    selector->addButton("Inductor",
                        ":/rcs/images/inductor.png",
                        ":/rcs/images/inductorSelected.png",
                        ":/rcs/images/inductorShadow.png",
                        true, false, "L", "H", 1);
    selector->addButton("Ground",
                        ":/rcs/images/ground.png",
                        ":/rcs/images/groundSelected.png",
                        ":/rcs/images/groundShadow.png",
                        false, false, "", "", 0.5);
    selector->addButton("Pressure",
                        ":/rcs/images/voltageSource.png",
                        ":/rcs/images/voltageSourceSelected.png",
                        ":/rcs/images/voltageSourceShadow.png",
                        true, true, "V", "V", 1);
    selector->addButton("Flow",
                        ":/rcs/images/currentSource.png",
                        ":/rcs/images/currentSourceSelected.png",
                        ":/rcs/images/currentSourceShadow.png",
                        true, true, "Q", "A", 1);


    QWidget *simulator = new QWidget();
    QPushButton *simulateButton = new QPushButton("Run Simulation");
    simulateButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(simulateButton, &QPushButton::clicked, schem, &Schematic::simulate); // TODO: connect to right function here
    QHBoxLayout *simLayout = new QHBoxLayout;
    simLayout->addWidget(simulateButton);
    simulator->setLayout(simLayout);

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
    addDockWidget(Qt::LeftDockWidgetArea,
                  dockSelector);

    QDockWidget *dockSimulator = new QDockWidget;
    dockSimulator->setWidget(simulator);
    dockSimulator->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea,
                  dockSimulator);
    setCorner(Qt::BottomLeftCorner, Qt::BottomDockWidgetArea);
}

MainWindow::~MainWindow()
{

}
