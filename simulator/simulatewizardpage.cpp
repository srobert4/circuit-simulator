#include "simulatewizardpage.h"

SimulateWizardPage::SimulateWizardPage(SpiceEngine *engine, Netlist *netlist, QWidget *parent) : QWizardPage(parent)
{
    setTitle("Run your simulation");
    this->netlist = netlist;
    this->engine = engine;

    QPushButton *runButton = new QPushButton("Click to Start Simulation", this);

    QWidget *progressWidget = new QWidget(this);
    progressBar = new QProgressBar(this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    QHBoxLayout *progressLayout = new QHBoxLayout;
    progressLayout->addWidget(progressBar);
    progressBar->setRange(0, 100);
    progressLayout->addWidget(cancelButton);
    progressWidget->setLayout(progressLayout);
    progressWidget->setHidden(true);

    connect(runButton, &QPushButton::pressed, [=](){
        runButton->setHidden(true);
        progressWidget->setVisible(true);
        runSimulation();
    });
    connect(cancelButton, &QPushButton::pressed, [=](){
        runButton->setVisible(true);
        progressWidget->setHidden(true);
        cancelSimulation();
    });


    layout = new QVBoxLayout;
    layout->addWidget(runButton);
    layout->addWidget(progressWidget);
    setLayout(layout);
}

void SimulateWizardPage::runSimulation()
{
    running = true;
    connect(engine, &SpiceEngine::statusUpdate, this, &SimulateWizardPage::updateProgressBar);
    if (field("loadCircuit").toBool()) {
        engine->startSimulation(field("filename").toString(), nullptr);
    } else {
        engine->startSimulation(netlist->getFilename(), netlist->getBoundaryConditions());
    }
}

void SimulateWizardPage::cancelSimulation()
{
    engine->stopSimulation();
    running = false;
}

void SimulateWizardPage::updateProgressBar(int status)
{
    progressBar->setValue(status);
    emit completeChanged();
}

bool SimulateWizardPage::isComplete() const
{
    if (!running) return true;
    return (progressBar->value() == 100);
}

bool SimulateWizardPage::validatePage()
{
    if (isComplete())
        engine->stopSimulation();
    return isComplete();
}
