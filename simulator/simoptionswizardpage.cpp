#include "simoptionswizardpage.h"

SimOptionsWizardPage::SimOptionsWizardPage(QGraphicsScene *schem, Netlist *netlist, bool saveOnly, QWidget *parent) : QWizardPage(parent)
{
    schematic = schem;
    this->netlist = netlist;
    this->saveOnly = saveOnly;
    setTitle("Set Simulation Mode");

    QComboBox *simulationTypeComboBox = new QComboBox(this);
    modes = {""};
    modes.append(simulationModes.keys());
    simulationTypeComboBox->addItems(modes);
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

    const QChar MathSymbolSigma(0x03BC);
    mu.setUnicode(&MathSymbolSigma, 1);
    unitModifiers = { "", "T", "G", "M", "K", "m", mu, "n", "p", "f" };

    QWidget *tran = createTranExtension();
    tran->setHidden(true);
    layout->addWidget(tran);
    int tranIndex = modes.indexOf("Transient");
    simulationExtensions[tranIndex] = tran;


    outputLine = new QLineEdit();
    registerField("simOptions", outputLine);

    setLayout(layout);
}

void SimOptionsWizardPage::initializePage()
{
    if (!saveOnly)
        connect(this, SIGNAL(parseSchematic()), schematic, SLOT(parseSchematic()));
    emit parseSchematic();

    QApplication::processEvents();

    QWidget *dc = createDCExtension(netlist->getElementNames());
    dc->setHidden(true);
    layout->addWidget(dc);
    int dcIndex = modes.indexOf("DC");
    simulationExtensions[dcIndex] = dc;

    disconnect(this, SIGNAL(parseSchematic()), schematic, SLOT(parseSchematic()));
}

QWidget *SimOptionsWizardPage::createTranExtension()
{
    QWidget *tran = new QWidget(this);
    QLabel *stepLabel = new QLabel("Step: ", this);
    QLabel *unitsLabel = new QLabel("s", this);
    QLabel *durationLabel = new QLabel("Duration: ", this);
    QLabel *unitsLabelTwo = new QLabel("s", this);

    QComboBox *stepUnits = new QComboBox(this);
    stepUnits->addItems(unitModifiers);
    registerField("tranStepUnits", stepUnits, "currentText");

    QComboBox *durUnits = new QComboBox(this);
    durUnits->addItems(unitModifiers);
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
    startUnits->addItems(unitModifiers);
    registerField("dcStartUnits", startUnits, "currentText");
    QLabel *startVLabel = new QLabel("V", this);

    QLabel *endLabel = new QLabel("Stop voltage: ", this);
    QLineEdit *endLineEdit = new QLineEdit(this);
    registerField("dcStopVoltage", endLineEdit);
    connect(endLineEdit, &QLineEdit::textEdited, [this](){ emit completeChanged(); });
    QComboBox *endUnits = new QComboBox(this);
    endUnits->addItems(unitModifiers);
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

    if (mode == "Transient")
        return (field("tranStep").toString() != ""  &&
                field("tranDuration").toString() != "");

    if (mode == "DC")
        return (field("dcVoltageSource").toString() != "" &&
            field("dcStartVoltage").toString() != "" &&
            field("dcStopVoltage").toString() != "");

    return false;
}

bool SimOptionsWizardPage::validatePage()
{
    QString mode = field("simulationType").toString();
    if (mode == "") return false;
    mode = simulationModes[mode];

    if (mode == ".tran") {
        QString stepUnits = field("tranStepUnits").toString();
        if (stepUnits == mu) stepUnits = "u";
        QString durUnits = field("tranDurUnits").toString();
        if (durUnits == mu) durUnits = "u";
        outputLine->setText(mode + " " + field("tranStep").toString() + stepUnits + "s" +
                " " + field("tranDuration").toString() + durUnits + "s" + " uic");
    }

    if (mode == ".dc") {
        QString startUnits = field("dcStartUnits").toString();
        if (startUnits == mu) startUnits = "u";
        QString stopUnits = field("dcStopVoltage").toString();
        if (stopUnits == mu) stopUnits = "u";
        outputLine->setText(mode + " " + field("dcVoltageSource").toString() + " " +
                            field("dcStartVoltage").toString() + startUnits + " " +
                            field("dcStopVoltage").toString() + stopUnits);
    }
    return true;

}
