#include "simulatewizardpage.h"

simulateWizardPage::simulateWizardPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle("Run your simulation");
    QLabel *text = new QLabel("This will be the simulation page", this);
    layout = new QVBoxLayout;
    layout->addWidget(text);
    setLayout(layout);
}

void simulateWizardPage::initializePage()
{
    QString filename;
    if (field("loadCircuit").toBool()) {
        filename = field("filename").toString();
    } else {
        filename = field("saveDir").toString() + "/" +
                field("saveFilename").toString() + ".cir";
    }
    QLabel *label = new QLabel(filename, this);
    layout->addWidget(label);
    // initialise spiceengine with file
}
