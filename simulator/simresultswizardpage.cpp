#include "simresultswizardpage.h"

SimResultsWizardPage::SimResultsWizardPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle("View Your Results");
    QLabel *text = new QLabel("This will be the graphing options page", this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(text);
    setLayout(layout);
}
