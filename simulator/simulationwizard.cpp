#include "simulationwizard.h"

SimulationWizard::SimulationWizard(Netlist *netlist, bool saveOnly, QWidget *parent) : QWizard(parent)
{
    this->netlist = netlist;
    this->saveOnly = saveOnly;

    if (!saveOnly) {
        setPage(Page_Intro, new IntroWizardPage);
    }

    setPage(Page_SimOptions, createSimOptionsPage());

    setPage(Page_InitialConds, createInitialCondPage());

    SaveWizardPage *savePage = new SaveWizardPage;
    connect(savePage, &SaveWizardPage::ready,
            [this]() {
        this->processInput();
        this->netlist->writeToFile(this->filename);
    });

    setPage(Page_SaveAs, savePage);

    setPage(Page_RunSim, new simulateWizardPage);

    setPage(Page_GraphOptions, createGraphOptionsPage());

    setWindowTitle("Simulation Wizard");
    show();
}

QWizardPage *SimulationWizard::createSimOptionsPage()
{
    QWizardPage *page = new QWizardPage;
    page->setTitle("Set Simulation Mode");

    simulationTypeComboBox = new QComboBox(this);
    simulationTypeComboBox->addItems(simulationTypes);

    QFormLayout *layout = new QFormLayout;
    layout->addRow("Simulation mode: ", simulationTypeComboBox);

    // Set up extensions for each option
    tran = new QWidget(this);
    QLabel *stepLabel = new QLabel("Step: ", this);
    QLabel *unitsLabel = new QLabel("s", this);
    QLabel *durationLabel = new QLabel("Duration: ", this);
    QLabel *unitsLabelTwo = new QLabel("s", this);
    stepUnits = new QComboBox(this);
    stepUnits->addItems(timeUnits);
    durUnits = new QComboBox(this);
    durUnits->addItems(timeUnits);
    durationLineEdit = new QLineEdit(this);
    stepLineEdit = new QLineEdit(this);

    QGridLayout *tranLayout = new QGridLayout;
    tranLayout->addWidget(stepLabel, 0, 0);
    tranLayout->addWidget(stepLineEdit, 0, 1);
    tranLayout->addWidget(stepUnits, 0, 2);
    tranLayout->addWidget(unitsLabel, 0, 3);
    tranLayout->addWidget(durationLabel, 1, 0);
    tranLayout->addWidget(durationLineEdit, 1, 1);
    tranLayout->addWidget(durUnits, 1, 2);
    tranLayout->addWidget(unitsLabelTwo, 1, 3);
    tran->setLayout(tranLayout);
    tran->setHidden(true);

    connect(simulationTypeComboBox, SIGNAL ( currentIndexChanged(int) ),
            this, SLOT ( setSimulationOptionsView(int) ));

    layout->addWidget(tran);
    simulationExtensions[1] = tran;

    page->setLayout(layout);
    return page;
}

void SimulationWizard::setSimulationOptionsView(int index)
{
    if (index < 1) return;
    simulationExtensions[index]->setVisible(true);
}

QWizardPage *SimulationWizard::createInitialCondPage()
{

    QWizardPage *page = new QWizardPage;
    page->setTitle("Set Initial Conditions");

    QLabel *label = new QLabel("Each node is labelled on the schematic. For any nodes "
                               "for which you want to set the initial pressure, enter "
                               "the pressure value in the corresponding box below.", this);
    label->setWordWrap(true);

    QFormLayout *formLayout = new QFormLayout;
    foreach(QString node, netlist->getNodeNames()) {
        QLineEdit *line = new QLineEdit(this);
        formLayout->addRow("Node " + node, line);
        nodeNames.append(node);
        initialConditionLineEdits.append(line);
    }

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    layout->addLayout(formLayout);
    page->setLayout(layout);
    return page;
}

QWizardPage *SimulationWizard::createGraphOptionsPage()
{
    QWizardPage *page = new QWizardPage;
    QLabel *text = new QLabel("This will be the graphing options page", this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(text);
    page->setLayout(layout);
    return page;
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
        return Page_GraphOptions;

    case Page_GraphOptions:
    default:
        return -1;
    }
}

void SimulationWizard::processInput()
{
    if (field("loadCircuit").toBool()) return;
    netlist->setName(field("circuitName").toString());
    netlist->setAnalysis(
                simulationTypeComboBox->currentText(),
                stepLineEdit->text(), stepUnits->currentText() + "s",
                durationLineEdit->text(),
                durUnits->currentText() + "s");
    for (int i = 0; i < nodeNames.length(); i++){
        if (initialConditionLineEdits[i]->text() != "")
            netlist->addInitialCondition(nodeNames[i], initialConditionLineEdits[i]->text());
    }
    filename = field("saveDir").toString() + "/" + field("saveFilename").toString() + ".cir";
}
