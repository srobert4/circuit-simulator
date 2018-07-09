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
    Schematic *schem;
    ElementSelector *selector;

private slots:
    void slotSchematicClicked();
};

#endif // MAINWINDOW_H
