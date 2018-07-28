#ifndef SPICEENGINE_H
#define SPICEENGINE_H

#include <QObject>
#include <QWidget>
#include "netlist.h"
#include "ngspice/include/sharedspice.h"

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
    explicit SpiceEngine(Netlist *netlist, QObject *parent = nullptr);
    void runSimulation(QString filename, QMap<QString, QString> bcs);
    void cancelSimulation();
    QList<QString> getAvailableGraphs();
    void graph(QString node);

    bool no_bg = true;
    int vecgetnumber = 0;
    bool errorflag = false;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    Netlist *netlist;

private:

signals:

public slots:
};

#endif // SPICEENGINE_H
