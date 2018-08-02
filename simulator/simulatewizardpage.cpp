#include "simulatewizardpage.h"

SimulateWizardPage::SimulateWizardPage(SpiceEngine *engine, Netlist *netlist, QWidget *parent) : QWizardPage(parent)
{
    setTitle("Run your simulation");
    this->netlist = netlist;
    this->engine = engine;

    QWidget *runWidget = new QWidget(this);
    QPushButton *runButton = new QPushButton("Click to Start Simulation", this);
    QCheckBox *dumpOutputCheckbox = new QCheckBox("Write simulation output to file?", this);
    registerField("dumpOutput", dumpOutputCheckbox);
    dumpFilenameLineEdit = new QLineEdit(this);
    dumpFilenameLineEdit->setHidden(true);
    registerField("dumpFilename", dumpFilenameLineEdit);
    QVBoxLayout *runLayout = new QVBoxLayout;
    runLayout->addWidget(runButton);
    runLayout->addWidget(dumpOutputCheckbox);
    runLayout->addWidget(dumpFilenameLineEdit);
    runWidget->setLayout(runLayout);

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
        runWidget->setHidden(true);
        progressWidget->setVisible(true);
        runSimulation();
    });
    connect(cancelButton, &QPushButton::pressed, [=](){
        runWidget->setVisible(true);
        progressWidget->setHidden(true);
        cancelSimulation();
    });
    connect(dumpOutputCheckbox, &QCheckBox::stateChanged, [=](){
        if (dumpOutputCheckbox->isChecked()) {
            dumpFilenameLineEdit->setVisible(true);
        } else {
            dumpFilenameLineEdit->setHidden(true);
        }
    });

    connect(engine, &SpiceEngine::statusUpdate, this, &SimulateWizardPage::updateProgressBar);
    connect(engine, &SpiceEngine::spiceError, [=](char *errormsg){
        QMessageBox *box = new QMessageBox(QMessageBox::Critical, "Spice Error", errormsg);
        box->exec();
        delete box;
    });

    layout = new QVBoxLayout;
    layout->addWidget(runWidget);
    layout->addWidget(progressWidget);
    setLayout(layout);
}

void SimulateWizardPage::runSimulation()
{
    running = true;
    if (field("loadCircuit").toBool()) {
        // TODO: allow user to provide filename for any External input elements
        // create bcs map, and pass a pointer to this function.
        engine->startSimulation(field("filename").toString(), nullptr, field("dumpOutput").toBool(), field("dumpFilename").toString());
    } else {
        engine->startSimulation(netlist, field("dumpOutput").toBool(), field("dumpFilename").toString());
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

void SimulateWizardPage::initializePage()
{
    QString filename;
    if (field("loadCircuit").toBool()) {
        filename = field("filename").toString();
    } else {
        filename = netlist->getFilename();
    }
    filename = filename.left(filename.length() - 4) + "_dump.txt";
    dumpFilenameLineEdit->setText(filename);
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
