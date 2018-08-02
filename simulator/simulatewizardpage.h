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
    virtual void initializePage() override;

private:
    SpiceEngine *engine;
    Netlist *netlist;
    QVBoxLayout *layout;
    QProgressBar *progressBar;
    QLineEdit *dumpFilenameLineEdit;
    QLabel *resultsLabel;
    QPushButton *saveButton;
    QPushButton *plotButton;
    bool running;
    QList<QString> vectors;
    QMap<QString, bool> selectedVectors;
    QString defaultFilename;

    void runSimulation();
    void cancelSimulation();
    void showResults();
    void initData();
    void writeVectors();
    void plot();
    void save();

    QWidget *getPlotWidget();
    QWidget *getSaveWidget();

private slots:
    void updateStatus(int progress);
    void receiveError(char *errormsg);
};

#endif // SIMULATEWIZARDPAGE_H
