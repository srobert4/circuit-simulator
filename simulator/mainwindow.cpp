#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    schem = new Schematic(this);
    selector = new ElementSelector(this);
    setCentralWidget(schem);
    QDockWidget *dockSelector = new QDockWidget;
    dockSelector->setWidget(selector);
    dockSelector->setAllowedAreas(Qt::LeftDockWidgetArea);
    addDockWidget(
                Qt::LeftDockWidgetArea,
                dockSelector);

    selector->addButton("Resistor",
                        "/home/srobertson/Downloads/res.png");
    selector->addButton("Capacitor",
                        "/home/srobertson/Downloads/cap.png");
}

MainWindow::~MainWindow()
{

}

void MainWindow::slotSchematicClicked()
{
    QString path = selector->getElementPath();
    if (path == "") return;
    schem->addElement(path);
}
