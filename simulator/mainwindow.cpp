#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setFixedSize(500,500);
    schem = new Schematic(100, 0, 400, 500, this);
    selector = new ElementSelector(0, 0, 100, 500, this);
    selector->addButton("Resistor",
                        "/home/srobertson/Downloads/res.png",
                        10, 50, 80, 30);
    selector->addButton("Capacitor",
                        "/home/srobertson/Downloads/cap.jpg",
                        10, 100, 80, 30);
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
