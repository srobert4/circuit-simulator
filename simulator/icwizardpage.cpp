#include "icwizardpage.h"

ICWizardPage::ICWizardPage(Netlist *netlist, QWidget *parent) : QWizardPage(parent)
{
    this->netlist = netlist;
    setTitle("Set Initial Conditions");

    QLabel *label = new QLabel("Each node is labelled on the schematic. For any nodes "
                               "for which you want to set the initial pressure, enter "
                               "the pressure value in the corresponding box below.", this);
    label->setWordWrap(true);

    layout = new QVBoxLayout;
    layout->addWidget(label);

    outputLine = new QLineEdit();
    registerField("initialConditions", outputLine);

    setLayout(layout);
}

void ICWizardPage::initializePage()
{
    QFormLayout *formLayout = new QFormLayout;
    foreach(QString node, netlist->getNodeNames()) {
        QLineEdit *line = new QLineEdit(this);
        formLayout->addRow("Node " + node, line);
        registerField(node, line);
    }
    layout->addLayout(formLayout);

}


bool ICWizardPage::validatePage()
{
    QString line = ".ic";
    foreach(QString node, netlist->getNodeNames()) {
        QString value = field(node).toString();
        if (value == "") continue;
        bool numName;
        node.toInt(&numName);
        line += " " + (numName ? "v(" + node + ")" : node) + "=" + value;
    }
    if (line == ".ic") line = "";
    outputLine->setText(line);
    return true;
}
