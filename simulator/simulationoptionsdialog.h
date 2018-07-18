#ifndef SIMULATIONOPTIONSDIALOG_H
#define SIMULATIONOPTIONSDIALOG_H

#include <QtWidgets>
#include "netlist.h"

class SimulationOptionsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SimulationOptionsDialog(Netlist *netlist, QWidget *parent = nullptr);
    void processInput();
    QString getFilename() {return (filenameLineEdit->text() + ".cir");}

private:
    Netlist *netlist;

    QLineEdit *nameLineEdit;
    QLineEdit *filenameLineEdit;
    QComboBox *simulationTypeComboBox;
    QList<QString> simulationTypes = {
        ".tran"
    };
    QWidget *tranExt;
    QVector<QLineEdit *> initialConditionLineEdits;
    QVector<QLabel *> initialConditionLabels;
    QButtonGroup *nodesToGraph;
    QButtonGroup *elementsToGraph;

signals:

public slots:
};

#endif // SIMULATIONOPTIONSDIALOG_H
