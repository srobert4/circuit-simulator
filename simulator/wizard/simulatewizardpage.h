#ifndef SIMULATEWIZARDPAGE_H
#define SIMULATEWIZARDPAGE_H

#include <QtWidgets>
#include "../simulation/netlist.h"
#include "../simulation/spiceengine.h"

class SimulateWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit SimulateWizardPage(Netlist *netlist = nullptr,
                                QMap<QString, BoundaryCondition *> *bcMap = nullptr,
                                QWidget *parent = nullptr);

protected:
    virtual bool isComplete() const override;
    virtual void initializePage() override;

private:
    SpiceEngine *engine = nullptr;
    Netlist *netlist = nullptr;
    QMap<QString, BoundaryCondition *> *bcMap;

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

    void showErrorMessage();

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

    void parseBoundaryConditions(QString filename,
                                 QMap<QString, BoundaryCondition *> *bcMap);
    QString getFile(QString node);

private slots:
    void updateStatus(int progress);
    void receiveError(QString errormsg);
};

#endif // SIMULATEWIZARDPAGE_H
