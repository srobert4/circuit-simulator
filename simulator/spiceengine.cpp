#include "spiceengine.h"

SpiceEngine::SpiceEngine(Netlist *netlist, QObject *parent) : QObject(parent)
{
    this->netlist = netlist;
    int ret = ngSpice_Init(getchar, getstat, ng_exit, NULL, initdata, thread_runs, this);
    ret = ngSpice_Init_Sync(getvoltage, NULL, NULL, NULL, this);
}

/* Callback function called from bg thread in ngspice to transfer
any string created by printf or puts. Output to stdout in ngspice is
preceded by token stdout, same with stderr.*/
int getchar(char* outputreturn, int ident, void* userdata)
{
    Q_UNUSED(ident);
    SpiceEngine *engine = (SpiceEngine *)userdata;
    printf("%s\n", outputreturn);
    /* setting a flag if an error message occurred */
    if (ciprefix("stderr Error:", outputreturn))
        engine->errorflag = true;
    return 0;
}


int getstat(char* outputreturn, int ident, void* userdata)
{
    Q_UNUSED(ident);
    Q_UNUSED(userdata);
    printf("%s\n", outputreturn);
    return 0;
}

int thread_runs(bool noruns, int ident, void* userdata)
{
    Q_UNUSED(ident);
    SpiceEngine *engine = (SpiceEngine *)userdata;
    pthread_mutex_lock(&engine->mutex);
    engine->no_bg = noruns;
    if (noruns) {
        pthread_cond_signal(&engine->cond);
        printf("bg not running\n");
    } else {
        pthread_cond_signal(&engine->cond);
        printf("bg running\n");
    }
    pthread_mutex_unlock(&engine->mutex);
    return 0;
}


/* Callback function called from bg thread in ngspice once upon intialization
   of the simulation vectors)*/
int initdata(pvecinfoall intdata, int ident, void* userdata)
{
    Q_UNUSED(ident);
    SpiceEngine *engine = (SpiceEngine *)userdata;
    int i;
    int vn = intdata->veccount;
    for (i = 0; i < vn; i++) {
        printf("Vector: %s\n", intdata->vecs[i]->vecname);
        /* find the location of V(2) */
        if (cieq(intdata->vecs[i]->vecname, (char*) "V(2)"))
            engine->vecgetnumber = i;
    }
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
        //ngSpice_Command( (char*) "quit 5");
    }

    return exitstatus;
}

int getvoltage(double* voltage, double t, char* node, int ident, void* userdata)
{
    Q_UNUSED(ident);
    SpiceEngine *engine = (SpiceEngine *)userdata;
    *voltage = engine->netlist->getBoundaryPressure(node, t);
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

