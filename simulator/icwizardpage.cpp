#include "icwizardpage.h"

ICWizardPage::ICWizardPage(Netlist *netlist, QWidget *parent) : QWizardPage(parent)
{
    this->netlist = netlist;
    setTitle("Set Initial Conditions");

    QLabel *label = new QLabel("Each node is labelled on the schematic. "
                               "Set initial pressures below.", this);
    label->setWordWrap(true);

    layout = new QVBoxLayout;
    layout->addWidget(label);

    outputLine = new QLineEdit();
    registerField("initialConditions", outputLine);

    setLayout(layout);
}

// ============= PROTECTED FUNCTIONS ===========================================

/* inititalizePage()
 * -----------------
 * Once the netlist has been loaded, the lineedits
 * for each node can be added
 */
void ICWizardPage::initializePage()
{
    QFormLayout *formLayout = new QFormLayout;
    foreach(QString node, netlist->getNodeNames()) {
        QLineEdit *line = new QLineEdit(this);
        connect(line, &QLineEdit::textEdited, [this](){emit completeChanged();});
        formLayout->addRow("Node " + node, line);
        registerField(node, line);
    }
    layout->addLayout(formLayout);

}

/* isComplete()
 * ------------
 * Only allow the user to click next
 * if all lineEdits are empty or contain
 * a valid numeric string.
 */
bool ICWizardPage::isComplete() const
{
    bool ok = true;
    foreach(QString node, netlist->getNodeNames()) {
        QString value = field(node).toString();
        if (value.isEmpty()) continue;
        value.toDouble(&ok);
    }
    return ok;
}

/* validatePage()
 * --------------
 * Set outputLine text to ngspice command to set initial conditions
 */
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
