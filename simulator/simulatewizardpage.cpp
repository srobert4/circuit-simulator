#include "simulatewizardpage.h"

SimulateWizardPage::SimulateWizardPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle("Run your simulation");
    QPushButton *runButton = new QPushButton("Click to Start Simulation", this);

    QWidget *progressWidget = new QWidget(this);
    QProgressBar *progressBar = new QProgressBar(this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    QHBoxLayout *progressLayout = new QHBoxLayout;
    progressLayout->addWidget(progressBar);
    progressLayout->addWidget(cancelButton);
    progressWidget->setLayout(progressLayout);
    progressWidget->setHidden(true);

    connect(runButton, &QPushButton::pressed, [=](){
        runButton->setHidden(true);
        progressWidget->setVisible(true);
        runSimulation();
    });
    connect(cancelButton, &QPushButton::pressed, [=](){
        runButton->setVisible(true);
        progressWidget->setHidden(true);
        cancelSimulation();
    });


    layout = new QVBoxLayout;
    layout->addWidget(runButton);
    layout->addWidget(progressWidget);
    setLayout(layout);
}
