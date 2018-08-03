#include "saveintrowizardpage.h"

SaveIntroWizardPage::SaveIntroWizardPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle("Save Circuit");
    QLabel *text = new QLabel("Do you want to add simulation "
                              "settings to your saved file?");
    text->setWordWrap(true);
    QButtonGroup *answer = new QButtonGroup(this);
    QRadioButton *yes = new QRadioButton("Yes, add simulation settings", this);
    QRadioButton *no = new QRadioButton("No, save just the circuit", this);
    registerField("addSim", yes);
    no->setChecked(true);
    answer->addButton(yes);
    answer->addButton(no);
    answer->setExclusive(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(text);
    layout->addWidget(no);
    layout->addWidget(yes);
    setLayout(layout);
}
