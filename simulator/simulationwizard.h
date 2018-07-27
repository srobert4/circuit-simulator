#ifndef SIMULATIONOPTIONSDIALOG_H
#define SIMULATIONOPTIONSDIALOG_H

#include <QtWidgets>
#include "netlist.h"
#include "savewizardpage.h"
#include "introwizardpage.h"
#include "simulatewizardpage.h"

class SimulationWizard : public QWizard
{
    Q_OBJECT
public:
    enum { Page_Intro, Page_SimOptions, Page_InitialConds, Page_SaveAs,
           Page_RunSim, Page_GraphOptions };

    explicit SimulationWizard(Netlist *netlist, bool runSimulation, QWidget *parent = nullptr);
    void processInput();
    QString getFilename() { return filename; }

private:
    Netlist *netlist;
    QString filename;
    bool saveOnly;

    void createIntroPage();

    QWizardPage *createSimOptionsPage();
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

    QWizardPage *createInitialCondPage();
    QVector<QString> nodeNames;
    QVector<QLineEdit *> initialConditionLineEdits;

    QWizardPage *createSimulationPage();

    QWizardPage *createGraphOptionsPage();

    int nextId() const override;


signals:

public slots:
    void setSimulationOptionsView(int index);
};

#endif // SIMULATIONOPTIONSDIALOG_H
