#include "simoptionswizardpage.h"

SimOptionsWizardPage::SimOptionsWizardPage(QGraphicsScene *schem, Netlist *netlist, QWidget *parent) : QWizardPage(parent)
{
    schematic = schem;
    this->netlist = netlist;
    connect(this, SIGNAL(parseSchematic()), schematic, SLOT(parseSchematic()));
    setTitle("Set Simulation Mode");

    QComboBox *simulationTypeComboBox = new QComboBox(this);
    simulationTypeComboBox->addItems(simulationTypes);
    registerField("simulationType*", simulationTypeComboBox, "currentText");

    layout = new QFormLayout;
    layout->addRow("Simulation mode: ", simulationTypeComboBox);

    connect(simulationTypeComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int index){
        emit completeChanged();
        if (index >= 1) simulationExtensions[index]->setVisible(true);
        if (visibleExtension >= 1) simulationExtensions[visibleExtension]->setHidden(true);
        visibleExtension = index;}
    );

    QWidget *tran = createTranExtension();
    tran->setHidden(true);
    layout->addWidget(tran);
    simulationExtensions[1] = tran;


    outputLine = new QLineEdit();
    registerField("simOptions", outputLine);

    setLayout(layout);
}

void SimOptionsWizardPage::initializePage()
{
    emit parseSchematic();

    QApplication::processEvents();

    QWidget *dc = createDCExtension(netlist->getElementNames());
    dc->setHidden(true);
    layout->addWidget(dc);
    simulationExtensions[2] = dc;
}

QWidget *SimOptionsWizardPage::createTranExtension()
{
    QWidget *tran = new QWidget(this);
    QLabel *stepLabel = new QLabel("Step: ", this);
    QLabel *unitsLabel = new QLabel("s", this);
    QLabel *durationLabel = new QLabel("Duration: ", this);
    QLabel *unitsLabelTwo = new QLabel("s", this);

    QComboBox *stepUnits = new QComboBox(this);
    stepUnits->addItems(timeUnits);
    registerField("tranStepUnits", stepUnits, "currentText");

    QComboBox *durUnits = new QComboBox(this);
    durUnits->addItems(timeUnits);
    registerField("tranDurUnits", durUnits, "currentText");

    QLineEdit *durationLineEdit = new QLineEdit(this);
    registerField("tranDuration", durationLineEdit);
    connect(durationLineEdit, &QLineEdit::textEdited, [this](){ emit completeChanged(); });

    QLineEdit *stepLineEdit = new QLineEdit(this);
    registerField("tranStep", stepLineEdit);
    connect(stepLineEdit, &QLineEdit::textEdited, [this](){ emit completeChanged(); });

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
    return tran;
}

QWidget *SimOptionsWizardPage::createDCExtension(QSet<QString> elements)
{
    QWidget *dc = new QWidget(this);
    QList<QString> voltageSources;
    foreach(QString elem, elements) {
        if(elem.front() == "V")
            voltageSources.append(elem);
    }
    QLabel *sourceLabel = new QLabel("Voltage source: ", this);
    QComboBox *sourceComboBox = new QComboBox(this);
    sourceComboBox->addItems(voltageSources);
    registerField("dcVoltageSource", sourceComboBox, "currentText");

    QLabel *startLabel = new QLabel("Start voltage: ", this);
    QLineEdit *startLineEdit = new QLineEdit(this);
    registerField("dcStartVoltage", startLineEdit);
    connect(startLineEdit, &QLineEdit::textEdited, [this](){ emit completeChanged(); });
    QComboBox *startUnits = new QComboBox(this);
    startUnits->addItems(timeUnits);
    registerField("dcStartUnits", startUnits, "currentText");
    QLabel *startVLabel = new QLabel("V", this);

    QLabel *endLabel = new QLabel("Stop voltage: ", this);
    QLineEdit *endLineEdit = new QLineEdit(this);
    registerField("dcStopVoltage", endLineEdit);
    connect(endLineEdit, &QLineEdit::textEdited, [this](){ emit completeChanged(); });
    QComboBox *endUnits = new QComboBox(this);
    endUnits->addItems(timeUnits);
    registerField("dcStopUnits", endUnits, "currentText");
    QLabel *endVLabel = new QLabel("V", this);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(sourceLabel,0,0);
    layout->addWidget(sourceComboBox,0,1);
    layout->addWidget(startLabel,1,0);
    layout->addWidget(startLineEdit, 1, 1);
    layout->addWidget(startUnits, 1, 2);
    layout->addWidget(startVLabel, 1, 3);
    layout->addWidget(endLabel, 2, 0);
    layout->addWidget(endLineEdit, 2, 1);
    layout->addWidget(endUnits, 2, 2);
    layout->addWidget(endVLabel, 2, 3);

    dc->setLayout(layout);
    return dc;
}

bool SimOptionsWizardPage::isComplete() const
{
    QString mode = field("simulationType").toString();
    if (mode == "") return false;

    if (mode == ".tran")
        return (field("tranStep").toString() != ""  &&
                field("tranDuration").toString() != "");

    if (mode == ".dc")
        return (field("dcVoltageSource").toString() != "" &&
            field("dcStartVoltage").toString() != "" &&
            field("dcStopVoltage").toString() != "");

    return false;
}

bool SimOptionsWizardPage::validatePage()
{
    QString mode = field("simulationType").toString();
    if (mode == "") return false;

    if (mode == ".tran") {
        outputLine->setText(mode + " " + field("tranStep").toString() + field("tranStepUnits").toString() + "s" +
                " " + field("tranDuration").toString() + field("tranDurUnits").toString() + "s" + " uic");
    }

    if (mode == ".dc") {
        outputLine->setText(mode + " " + field("dcVoltageSource").toString() + " " +
                            field("dcStartVoltage").toString() + field("dcStartUnits").toString() + " " +
                            field("dcStopVoltage").toString() + field("dcStopUnits").toString());
    }
    return true;

}
