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
                        ":/images/resistor.png",
                        ":/images/resistorSelected.png",
                        ":/images/resistorShadow.png",
                        true, false, "R", units, 1);
    selector->addButton("Capacitor",
                        ":/images/capacitor.png",
                        ":/images/capacitorSelected.png",
                        ":/images/capacitorShadow.png",
                        true, false, "C", "F", 1);
    selector->addButton("Inductor",
                        ":/images/inductor.png",
                        ":/images/inductorSelected.png",
                        ":/images/inductorShadow.png",
                        true, false, "L", "H", 1);
    selector->addButton("Ground",
                        ":/images/ground.png",
                        ":/images/groundSelected.png",
                        ":/images/groundShadow.png",
                        false, false, "", "", 0.5);
    selector->addButton("Start Ground",
                        ":/images/startGround.png",
                        ":/images/startGroundSelected.png",
                        ":/images/startGroundShadow.png",
                        false, false, "", "", 0.5);
    selector->addButton("Pressure",
                        ":/images/voltageSource.png",
                        ":/images/voltageSourceSelected.png",
                        ":/images/voltageSourceShadow.png",
                        true, true, "V", "V", 1);
    selector->addButton("Flow",
                        ":/images/currentSource.png",
                        ":/images/currentSourceSelected.png",
                        ":/images/currentSourceShadow.png",
                        true, true, "Q", "A", 1);


    QWidget *simulator = new QWidget(this);
    QPushButton *simulateButton = new QPushButton("Run Simulation", simulator);
    simulateButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(simulateButton, &QPushButton::clicked, schem, &Schematic::simulate);
    QHBoxLayout *simLayout = new QHBoxLayout(simulator);
    simLayout->addWidget(simulateButton);
    simulator->setLayout(simLayout);

    // Set up schematic graphics view
    view = new QGraphicsView(schem, this);
    view->setMouseTracking(true);
    view->setDragMode(QGraphicsView::RubberBandDrag);
    schem->setSceneRect(0, 0, 800, 800); // set initial size
    setCentralWidget(view);

    // Set up selector dock
    QDockWidget *dockSelector = new QDockWidget(this);
    dockSelector->setWidget(selector);
    dockSelector->setAllowedAreas(Qt::LeftDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea,
                  dockSelector);

    QDockWidget *dockSimulator = new QDockWidget(this);
    dockSimulator->setWidget(simulator);
    dockSimulator->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea,
                  dockSimulator);
    setCorner(Qt::BottomLeftCorner, Qt::BottomDockWidgetArea);
}

MainWindow::~MainWindow()
{

}
