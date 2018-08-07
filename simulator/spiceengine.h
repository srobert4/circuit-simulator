#ifndef SPICEENGINE_H
#define SPICEENGINE_H

#include <stdio.h>
#include <QObject>
#include <QtWidgets>
#include "boundarycondition.h"
#include "/usr/local/include/ngspice/sharedspice.h"
#include "netlist.h"

int getchar(char *outputreturn, int ident, void *userdata);
int getstat(char *outputreturn, int ident, void *userdata);
int ng_exit(int, bool, bool, int ident, void *);
int thread_runs(bool noruns, int ident, void *userdata);
int initdata(pvecinfoall intdata, int ident, void *userdata);
int data(pvecvaluesall vdata, int numvecs, int ident, void *userdata);
int getvoltage(double *voltage, double t, char *node, int ident, void *userdata);

class SpiceEngine : public QObject
{
    Q_OBJECT
public:
    explicit SpiceEngine(QObject *parent = nullptr);
    int startSimulation(QString filename,
                         const QMap<QString, BoundaryCondition *> *bcs,
                         bool dump, QString dumpFilename);
    int startSimulation(Netlist *netlist, bool dump, QString dumpFilename);
    void _emitStatusUpdate(char *status);
    void _writeOutput(char *output);
    void _getVoltage(double *voltage, double t, char *node);
    bool running() { return ngSpice_running(); }
    int stopSimulation();
    int resumeSimulation();
    QString curPlot() { return QString(ngSpice_CurPlot()); }
    QList<QString> vectors();
    void _setVecInfo(pvecinfoall info);
    int saveResults(QList<QString> vecs, bool bin, QString filename);
    int plotResults(QList<QString> vecs, bool png, QString filename);
    bool getErrorStatus(QString &message);

    QString getPlotInfo() {
        return plotName + ": \n" +
                "Circuit: " + plotTitle + "\n" +
                "Date: " + plotDate + "\n" +
                "Plot: " + plotType; }
    bool no_bg = true;
    bool errorflag = false;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

private:
    // convenience wrappers around ngspice functions
    int command(QString command)
    {
        return ngSpice_Command(const_cast<char *>(command.toLatin1().data()));
    }
    int run() { return ngSpice_Command(const_cast<char *>("bg_run")); }
    int halt() { return ngSpice_Command(const_cast<char *>("bg_halt")); }
    int resume() { return ngSpice_Command(const_cast<char *>("bg_resume")); }

    const QMap<QString, BoundaryCondition *> *bcs;
    QString filename;
    bool dump = false;
    QString dumpFilename;
    Netlist *netlist = nullptr;

    // error handling
    void setErrorFlag(QString message);
    bool errorFlag = false;
    QString errorMsg;

    // plot info
    QString plotName;
    QString plotTitle;
    QString plotDate;
    QString plotType;
    int numVectors;
    QList<pvecinfo> vectorInfo;

signals:
    void statusUpdate(int status);
    void spiceError(QString errormsg);
    void initDataReady();

public slots:
};

#endif // SPICEENGINE_H
