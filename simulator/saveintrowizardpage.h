#ifndef SAVEINTROWIZARDPAGE_H
#define SAVEINTROWIZARDPAGE_H

#include <QtWidgets>

/* Class: SaveIntroWizardPage
 * --------------------------
 * Simple introduction page asking
 * for a yes/no answer to adding
 * simulation settings to saved netlist.
 */
class SaveIntroWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    /* Constructor */
    explicit SaveIntroWizardPage(QWidget *parent = nullptr) : QWizardPage (parent)
    {
        setTitle("Save Circuit");
        QLabel *text = new QLabel("Do you want to add simulation "
                                  "settings to your saved file?");
        text->setWordWrap(true);

        // Exclusive button group
        QButtonGroup *answer = new QButtonGroup(this);
        QRadioButton *yes = new QRadioButton("Yes, add simulation settings", this);
        QRadioButton *no = new QRadioButton("No, save just the circuit", this);
        registerField("addSim", yes);
        no->setChecked(true);
        answer->addButton(yes);
        answer->addButton(no);
        answer->setExclusive(true);

        // Set up layout
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(text);
        layout->addWidget(no);
        layout->addWidget(yes);
        setLayout(layout);
    }

};

#endif // SAVEINTROWIZARDPAGE_H
