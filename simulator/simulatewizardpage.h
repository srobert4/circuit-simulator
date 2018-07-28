#ifndef SIMULATEWIZARDPAGE_H
#define SIMULATEWIZARDPAGE_H

#include <QtWidgets>

class SimulateWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit SimulateWizardPage(QWidget *parent = nullptr);

private:
    QVBoxLayout *layout;
    void runSimulation() {}
    void cancelSimulation() {}

public slots:
};

#endif // SIMULATEWIZARDPAGE_H
