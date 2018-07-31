#ifndef SPICEENGINE_H
#define SPICEENGINE_H

#include <QObject>
#include <QtWidgets>
#include "boundarycondition.h"
#include "/usr/local/include/ngspice/sharedspice.h"

int getchar(char *outputreturn, int ident, void *userdata);
int getstat(char *outputreturn, int ident, void *userdata);
int ng_exit(int, bool, bool, int ident, void *);
int thread_runs(bool noruns, int ident, void *userdata);
int initdata(pvecinfoall intdata, int ident, void *userdata);
int data(pvecvaluesall vdata, int numvecs, int ident, void *userdata);
int getvoltage(double *voltage, double t, char *node, int ident, void *userdata);
int cieq(register char *p, register char *s);
int ciprefix(const char *p, const char *s);

class SpiceEngine : public QObject
{
    Q_OBJECT
public:
    explicit SpiceEngine(QObject *parent = nullptr);
    void startSimulation(QString filename, const QMap<QString, BoundaryCondition *> *bcs);
    void emitStatusUpdate(char *status);
    bool running() { return no_bg; }
    void stopSimulation();
    QList<QString> getAvailableGraphs();
    void graph(QString node);

    // convenience wrappers around ngspice functions
    int command(QString command) { return ngSpice_Command((char *)command.toLatin1().data()); }
    int run() { return ngSpice_Command((char *)"bg_run"); }
    int halt() { return ngSpice_Command((char *)"bg_halt"); }
    int resume() { return ngSpice_Command((char *)"bg_resume"); }

    bool no_bg = true;
    int vecgetnumber = 0;
    bool errorflag = false;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    QMap<QString, BoundaryCondition *> *bcs;
    QString filename;

signals:
    void statusUpdate(int status);

public slots:
};

#endif // SPICEENGINE_H
