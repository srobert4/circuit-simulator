#include "spiceengine.h"

SpiceEngine::SpiceEngine(QObject *parent) : QObject(parent)
{
    int ret;
    ret = ngSpice_Init(getchar, getstat, ng_exit, NULL, NULL, thread_runs, this);
    ret = ngSpice_Init_Sync(getvoltage, NULL, NULL, NULL, this);
}

void SpiceEngine::startSimulation(QString filename,
                                  const QMap<QString, BoundaryCondition *> *bcs,
                                  bool dump, QString dumpFilename)
{
    this->dump = dump;
    this->dumpFilename = dumpFilename;
    int ret = command("source " + filename);
    pthread_mutex_lock(&mutex);
    ret = run();
    // wait for background thread to start
    while(no_bg) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);
}

void SpiceEngine::startSimulation(Netlist *netlist, bool dump, QString dumpFilename)
{
    this->dump = dump;
    this->dumpFilename = dumpFilename;
    filename = netlist->getFilename();
    this->netlist = netlist;
    int ret = command("source " + filename);
    pthread_mutex_lock(&mutex);
    ret = run();
    // wait for background thread to start
    while(no_bg) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);
}

void SpiceEngine::stopSimulation()
{
    if (no_bg) return;
    halt(); // note: halt will not return until no_bg == true
}

void SpiceEngine::emitStatusUpdate(char *status)
{
    QString line = QString(status);
    QStringList tokens = line.split(" ");
    if (tokens.length() < 2) {
        if (tokens[0] == "--ready--")
            emit statusUpdate(100);
        return;
    }
    qreal stat = tokens[1].left(tokens[1].length() - 1).toDouble();
    if (stat != 0.0) emit statusUpdate((int)stat);
}

void SpiceEngine::writeOutput(char *output)
{
    if (ciprefix("stderr Error:", output))
        emit spiceError(output);
    if (!dump) return;
    QFile file(dumpFilename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << output << endl;
        file.close();
    }
}

void SpiceEngine::getVoltage(double *voltage, double t, char *node)
{
    if (netlist == nullptr) {
        *voltage = bcs->value(node)->getState(t);
    } else {
        *voltage = netlist->getBoundaryPressure(node, t);
    }
}

// ============= NGSPICE CALLBACK FUNCTIONS ======================

/* Callback function called from bg thread in ngspice to transfer
any string created by printf or puts. Output to stdout in ngspice is
preceded by token stdout, same with stderr.*/
int getchar(char* outputreturn, int ident, void* userdata)
{
    Q_UNUSED(ident);
    SpiceEngine *engine = (SpiceEngine *)userdata;
    engine->writeOutput(outputreturn);
    return 0;
}


int getstat(char* outputreturn, int ident, void* userdata)
{
    Q_UNUSED(ident);
    SpiceEngine *engine = (SpiceEngine *)userdata;
    engine->emitStatusUpdate(outputreturn);
    return 0;
}

int thread_runs(bool noruns, int ident, void* userdata)
{
    Q_UNUSED(ident);
    SpiceEngine *engine = (SpiceEngine *)userdata;
    pthread_mutex_lock(&engine->mutex);
    engine->no_bg = noruns;
    pthread_cond_signal(&engine->cond);
    pthread_mutex_unlock(&engine->mutex);
    return 0;
}


/* Callback function called from bg thread in ngspice if fcn controlled_exit()
   is hit. Do not exit, but unload ngspice. */
int ng_exit(int exitstatus, bool immediate, bool quitexit, int ident, void* userdata)
{
    Q_UNUSED(ident);
    SpiceEngine *engine = (SpiceEngine *)userdata;
    if(quitexit) {
        printf("DNote: Returned form quit with exit status %d\n", exitstatus);
        exit(exitstatus);
    }
    if(immediate) {
        printf("DNote: Unloading ngspice inmmediately is not possible\n");
        printf("DNote: Can we recover?\n");
    }

    else {
        printf("DNote: Unloading ngspice is not possible\n");
        printf("DNote: Can we recover? Send 'quit' command to ngspice.\n");
        engine->errorflag = true;
        ngSpice_Command( (char*) "quit 5");
    }

    return exitstatus;
}

int getvoltage(double* voltage, double t, char* node, int ident, void* userdata)
{
    Q_UNUSED(ident);
    SpiceEngine *engine = (SpiceEngine *)userdata;
    engine->getVoltage(voltage, t, node);
    return 0;
}

/* Case insensitive str eq. */
/* Like strcasecmp( ) XXX */
int cieq(register char *p, register char *s)
{
    while (*p) {
        if ((isupper(*p) ? tolower(*p) : *p) !=
            (isupper(*s) ? tolower(*s) : *s))
            return(false);
        p++;
        s++;
    }
    return (*s ? false : true);
}

/* Case insensitive prefix. */
int ciprefix(const char *p, const char *s)
{
    while (*p) {
        if ((isupper(*p) ? tolower(*p) : *p) !=
            (isupper(*s) ? tolower(*s) : *s))
            return(false);
        p++;
        s++;
    }
    return (true);
}

