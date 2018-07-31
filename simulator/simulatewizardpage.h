#ifndef SIMULATEWIZARDPAGE_H
#define SIMULATEWIZARDPAGE_H

#include <QtWidgets>
#include "netlist.h"
#include "spiceengine.h"

class SimulateWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit SimulateWizardPage(SpiceEngine *engine = nullptr,
                                Netlist *netlist = nullptr,
                                QWidget *parent = nullptr);

protected:
    virtual bool isComplete() const override;
    virtual bool validatePage() override;

private:
    SpiceEngine *engine;
    Netlist *netlist;
    QVBoxLayout *layout;
    QProgressBar *progressBar;
    bool running;

    void runSimulation();
    void cancelSimulation();

public slots:
    void updateProgressBar(int status);
};

#endif // SIMULATEWIZARDPAGE_H
