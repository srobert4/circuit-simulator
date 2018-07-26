#include "simulationwizard.h"

SimulationWizard::SimulationWizard(Netlist *netlist, QWidget *parent) : QWizard(parent)
{
    this->netlist = netlist;

    addPage(createSimulationPage());
    addPage(createInitialCondPage());
    addPage(createSavePage());

    setWindowTitle("Simulation Wizard");
    show();
}

QWizardPage *SimulationWizard::createSimulationPage()
{
    QWizardPage *page = new QWizardPage;
    page->setTitle("Simulation Mode");

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
    page->setTitle("Initial Conditions");

    QLabel *label = new QLabel("Each node is labelled on the schematic. For any nodes "
                               "for which you want to set the initial pressure, enter "
                               "the pressure value in the corresponding box below.", this);
    label->setWordWrap(true);

    QFormLayout *formLayout = new QFormLayout;
    foreach(QString node, netlist->getNodeNames()) {
        QLineEdit *line = new QLineEdit(this);
        formLayout->addRow("Node " + node, line);
        initialConditionLineEdits.append(line);
    }

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    layout->addLayout(formLayout);
    page->setLayout(layout);
    return page;
}

QWizardPage *SimulationWizard::createSavePage()
{
    QWizardPage *page = new QWizardPage;
    page->setTitle("Save Circuit");

    QLabel *label = new QLabel("Save your circuit to a file so that you can run"
                               " this simulation again without drawing a new schematic."
                               " Leave these fields blank to skip saving.", this);
    label->setWordWrap(true);

    nameLineEdit = new QLineEdit(this);

    QWidget *browser = new QWidget(this);
    saveDirLineEdit = new QLineEdit(this);
    QPushButton *browseButton = new QPushButton("Browse", this);
    QHBoxLayout *browserLayout = new QHBoxLayout;

    connect(browseButton, &QPushButton::pressed,
            [=](){ saveDirLineEdit->setText( QFileDialog::getExistingDirectory(
                                                 browser,
                                                 "Choose save directory",
                                                 "/home/srobertson",
                                                 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                                                 )); });

    browserLayout->addWidget(saveDirLineEdit);
    browserLayout->addWidget(browseButton);
    browser->setLayout(browserLayout);

    QWidget *filename = new QWidget(this);
    filenameLineEdit = new QLineEdit(this);
    QLabel *ext = new QLabel(".cir", this);
    QHBoxLayout *filenameLayout = new QHBoxLayout;
    filenameLayout->addWidget(filenameLineEdit);
    filenameLayout->addWidget(ext);
    filename->setLayout(filenameLayout);

    QFormLayout *layout = new QFormLayout;
    layout->addRow(label);
    layout->addRow("Model name: ", nameLineEdit);
    layout->addRow("Choose directory to save circuit in: ", browser);
    layout->addRow("Save circuit as: ", filename);
    layout->setRowWrapPolicy(QFormLayout::WrapAllRows);

    page->setLayout(layout);
    return page;
}

void SimulationWizard::processInput()
{
    netlist->setName(nameLineEdit->text());
    netlist->setAnalysis(simulationTypeComboBox->currentText(), "0.01", "ms", "5", "s");
//    for (int i = 0; i < initialConditionLabels.length(); i++){
//        if (initialConditionLineEdits[i]->text() != "")
//            netlist->addInitialCondition(initialConditionLabels[i]->text(), initialConditionLineEdits[i]->text());
//    }
}
