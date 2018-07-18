#include "simulationoptionsdialog.h"

SimulationOptionsDialog::SimulationOptionsDialog(Netlist *netlist, QWidget *parent) : QDialog(parent)
{
    this->netlist = netlist;

    QLabel *nameLabel = new QLabel("Model name: ");
    nameLineEdit = new QLineEdit;

    QLabel *filenameLabel = new QLabel("Save circuit as: ");
    filenameLineEdit = new QLineEdit;
    QLabel *filenameExtLabel = new QLabel(".cir");

    QLabel *simTypeLabel = new QLabel("Simulation type: ");
    simulationTypeComboBox = new QComboBox;
    simulationTypeComboBox->addItems(simulationTypes);

    nodesToGraph = new QButtonGroup;
    nodesToGraph->setExclusive(false);
    elementsToGraph = new QButtonGroup;
    elementsToGraph->setExclusive(false);
    for (QString node : netlist->getNodeNames()) {
        nodesToGraph->addButton(new QCheckBox(node));
        initialConditionLineEdits.append(new QLineEdit());
        initialConditionLabels.append(new QLabel("v(" + node + ")"));
    }

    for (QString element : netlist->getElementNames()) {
        elementsToGraph->addButton(new QCheckBox(element));
    }

    QGridLayout *topLayout = new QGridLayout;
    topLayout->addWidget(nameLabel, 0, 0);
    topLayout->addWidget(nameLineEdit, 0, 1, 1, 2);

    topLayout->addWidget(filenameLabel, 1, 0);
    topLayout->addWidget(filenameLineEdit, 1, 1);
    topLayout->addWidget(filenameExtLabel, 1, 2);

    topLayout->addWidget(simTypeLabel, 2, 0);
    topLayout->addWidget(simulationTypeComboBox, 2, 1, 1, 2);

    QLabel *nodesLabel = new QLabel("Nodes:");
    QLabel *elementLabel = new QLabel("Elements:");
    QGridLayout *bottomLayout = new QGridLayout;

    bottomLayout->addWidget(nodesLabel, 0, 0, 1, 3);
    bottomLayout->addWidget(elementLabel, 0, 3);
    int row = 1;
    for(QAbstractButton *checkbox : nodesToGraph->buttons()) {
        bottomLayout->addWidget(checkbox, row, 0);
        row++;
    }
    row = 1;
    for(QLabel *label : initialConditionLabels) {
        bottomLayout->addWidget(label, row, 1, Qt::AlignRight);
        row++;
    }
    row = 1;
    for (QLineEdit *lineEdit : initialConditionLineEdits) {
        bottomLayout->addWidget(lineEdit, row, 2);
        row++;
    }
    row = 1;
    for (QAbstractButton *checkbox : elementsToGraph->buttons()) {
        bottomLayout->addWidget(checkbox, row, 3);
        row++;
    }

    QPushButton *cancelButton = new QPushButton("Cancel");
    QPushButton *doneButton  = new QPushButton("Run");
    doneButton->setDefault(true);
    bottomLayout->addWidget(cancelButton, row, 2);
    bottomLayout->addWidget(doneButton, row, 3);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(bottomLayout);
    setLayout(mainLayout);

    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(doneButton, &QPushButton::clicked, this, &QDialog::accept);
}

void SimulationOptionsDialog::processInput()
{
    netlist->setName(nameLineEdit->text());
    netlist->setAnalysis(simulationTypeComboBox->currentText(), "0.01", "ms", "5", "s");
    for (int i = 0; i < initialConditionLabels.length(); i++){
        if (initialConditionLineEdits[i]->text() != "")
            netlist->addInitialCondition(initialConditionLabels[i]->text(), initialConditionLineEdits[i]->text());
    }
}
