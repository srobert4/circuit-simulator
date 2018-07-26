#ifndef SIMULATIONOPTIONSDIALOG_H
#define SIMULATIONOPTIONSDIALOG_H

#include <QtWidgets>
#include "netlist.h"

class SimulationWizard : public QWizard
{
    Q_OBJECT
public:
    explicit SimulationWizard(Netlist *netlist, QWidget *parent = nullptr);
    void processInput();
    QString getFilename() {return (filenameLineEdit->text() + ".cir");}

private:
    Netlist *netlist;

    QWizardPage *createSimulationPage();
    QWizardPage *createInitialCondPage();
    QWizardPage *createSavePage();


    QLineEdit *nameLineEdit;
    QLineEdit *saveDirLineEdit;
    QLineEdit *filenameLineEdit;
    QComboBox *simulationTypeComboBox;
    QList<QString> simulationTypes = {
        "",
        ".tran"
    };
    QMap<int, QWidget *> simulationExtensions;
    QWidget *tran;
    QLineEdit *durationLineEdit;
    QLineEdit *stepLineEdit;
    QList<QString> timeUnits = {
        "", "m", "u", "n"
    };
    QComboBox *stepUnits;
    QComboBox *durUnits;
    QVector<QLineEdit *> initialConditionLineEdits;

signals:

public slots:
    void setSimulationOptionsView(int index);
};

#endif // SIMULATIONOPTIONSDIALOG_H
