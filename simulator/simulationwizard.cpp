#include "simulationwizard.h"

SimulationWizard::SimulationWizard(Netlist *netlist, SpiceEngine *engine, bool saveOnly, QGraphicsScene *schem, QWidget *parent) : QWizard(parent)
{
    this->netlist = netlist;
    this->saveOnly = saveOnly;

    if (!saveOnly) {
        setPage(Page_Intro, new IntroWizardPage);
    }

    setPage(Page_SimOptions, new SimOptionsWizardPage(schem, netlist));

    setPage(Page_InitialConds, new ICWizardPage(netlist));

    SaveWizardPage *savePage = new SaveWizardPage(saveOnly);
    connect(savePage, &SaveWizardPage::ready,
            [this]() {
        this->processInput();
        this->netlist->writeToFile(this->filename);
    });
    setPage(Page_SaveAs, savePage);

    setPage(Page_RunSim, new SimulateWizardPage(engine, netlist));

    setWindowTitle("Simulation Wizard");
    show();
}

int SimulationWizard::nextId() const
{
    switch(currentId())
    {
    case Page_Intro:
        if (field("parseCircuit").toBool())
            return Page_SimOptions;
        return Page_RunSim;

    case Page_SimOptions:
        return Page_InitialConds;
    case Page_InitialConds:
        return Page_SaveAs;
    case Page_SaveAs:
        if (saveOnly) return -1;
        return Page_RunSim;

    case Page_RunSim:
    default:
        return -1;
    }
}

void SimulationWizard::processInput()
{
    if (!this->saveOnly && field("loadCircuit").toBool()) return;
    netlist->setName(field("circuitName").toString());
    netlist->setAnalysis(field("simOptions").toString());
    netlist->setInitialConditions(field("initialConditions").toString());
    filename = field("saveDir").toString() + "/" + field("saveFilename").toString() + ".cir";
}
