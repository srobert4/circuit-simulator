#include "simoptionswizardpage.h"

SimOptionsWizardPage::SimOptionsWizardPage(QGraphicsScene *schem, QWidget *parent) : QWizardPage(parent)
{
    schematic = schem;
    connect(this, SIGNAL(parseSchematic()), schematic, SLOT(parseSchematic()));
    setTitle("Set Simulation Mode");

    QComboBox *simulationTypeComboBox = new QComboBox(this);
    simulationTypeComboBox->addItems(simulationTypes);
    registerField("simulationType*", simulationTypeComboBox, "currentText");

    QFormLayout *layout = new QFormLayout;
    layout->addRow("Simulation mode: ", simulationTypeComboBox);

    connect(simulationTypeComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int index){
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
    QEventLoop loop;
    connect(schematic, SIGNAL(parseComplete()), &loop, SLOT(quit()));
    loop.exec(); // wait for parsing to finish
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
    registerField("tranDuration*", durationLineEdit);

    QLineEdit *stepLineEdit = new QLineEdit(this);
    registerField("tranStep*", stepLineEdit);

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

bool SimOptionsWizardPage::validatePage()
{
    QString mode = field("simulationType").toString();
    if (mode == "") return false;

    if (mode == ".tran") {
        outputLine->setText(mode + " " + field("tranStep").toString() + field("tranStepUnits").toString() + "s" +
                " " + field("tranDuration").toString() + field("tranDurUnits").toString() + "s");
    }
    return true;

}
