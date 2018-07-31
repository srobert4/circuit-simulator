#include "spiceengine.h"

SpiceEngine::SpiceEngine(QObject *parent) : QObject(parent)
{
    int ret;
    ret = ngSpice_Init(getchar, getstat, ng_exit, NULL, NULL, thread_runs, this);
    ret = ngSpice_Init_Sync(getvoltage, NULL, NULL, NULL, this);
}

void SpiceEngine::startSimulation(QString filename, const QMap<QString, BoundaryCondition *> *bcs)
{
    int ret;
    ret = command("source " + filename);
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

// ============= NGSPICE CALLBACK FUNCTIONS ======================

/* Callback function called from bg thread in ngspice to transfer
any string created by printf or puts. Output to stdout in ngspice is
preceded by token stdout, same with stderr.*/
int getchar(char* outputreturn, int ident, void* userdata)
{
    Q_UNUSED(ident);
    SpiceEngine *engine = (SpiceEngine *)userdata;
    if (ciprefix("stderr Error:", outputreturn))
        engine->errorflag = true;
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
    *voltage = engine->bcs->value(node)->getState(t);
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

