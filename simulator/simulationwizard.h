#ifndef SIMULATIONOPTIONSDIALOG_H
#define SIMULATIONOPTIONSDIALOG_H

#include <QtWidgets>
#include "netlist.h"
#include "savewizardpage.h"
#include "introwizardpage.h"
#include "simulatewizardpage.h"
#include "simoptionswizardpage.h"
#include "icwizardpage.h"
#include "simresultswizardpage.h"

class SimulationWizard : public QWizard
{
    Q_OBJECT
public:
    enum { Page_Intro, Page_SimOptions, Page_InitialConds, Page_SaveAs, Page_RunSim };

    explicit SimulationWizard(Netlist *netlist,
                              SpiceEngine *engine,
                              bool saveOnly,
                              QGraphicsScene *schem,
                              QWidget *parent = nullptr);
    void processInput();
    QString getFilename() { return filename; }

private:
    Netlist *netlist;
    QString filename;
    bool saveOnly;

    int nextId() const override;


signals:
};

#endif // SIMULATIONOPTIONSDIALOG_H
