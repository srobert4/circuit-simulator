#include "icwizardpage.h"

ICWizardPage::ICWizardPage(QSet<QString> nodeNames, QWidget *parent) : QWizardPage(parent)
{
    this->nodes = nodeNames;
    setTitle("Set Initial Conditions");

    QLabel *label = new QLabel("Each node is labelled on the schematic. For any nodes "
                               "for which you want to set the initial pressure, enter "
                               "the pressure value in the corresponding box below.", this);
    label->setWordWrap(true);

    QFormLayout *formLayout = new QFormLayout;
    foreach(QString node, nodes) {
        QLineEdit *line = new QLineEdit(this);
        formLayout->addRow("Node " + node, line);
        registerField(node, line);
    }

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    layout->addLayout(formLayout);

    outputLine = new QLineEdit();
    registerField("initialConditions", outputLine);

    setLayout(layout);
}


bool ICWizardPage::validatePage()
{
    QString line = ".ic";
    foreach(QString node, nodes) {
        QString value = field(node).toString();
        if (value == "") continue;
        bool numName;
        node.toInt(&numName);
        line += " " + (numName ? "v(" + node + ")" : node) + "=" + value;
    }
    outputLine->setText(line);
    return true;
}
