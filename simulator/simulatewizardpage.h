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
    virtual void initializePage() override;

private:
    SpiceEngine *engine = nullptr;
    Netlist *netlist = nullptr;
    QVBoxLayout *layout = nullptr;
    QProgressBar *progressBar = nullptr;
    QLineEdit *dumpFilenameLineEdit = nullptr;
    QLabel *resultsLabel = nullptr;
    QPushButton *saveButton = nullptr;
    QPushButton *plotButton = nullptr;
    QPushButton *pauseButton = nullptr;
    bool running;
    bool resultsInitialized = false;
    QList<QString> vectors;
    QMap<QString, bool> selectedVectors;
    QString defaultFilename;

    void startSimulation();
    void continueSimulation();
    void stopSimulation();
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
