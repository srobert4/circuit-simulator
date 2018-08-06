#include "spiceengine.h"

SpiceEngine::SpiceEngine(QObject *parent) : QObject(parent)
{
    ngSpice_Init(getchar, getstat, ng_exit, nullptr, initdata, thread_runs, this);
    ngSpice_Init_Sync(getvoltage, nullptr, nullptr, nullptr, this);
}

void SpiceEngine::setErrorFlag(QString message)
{
    errorFlag = true;
    errorMsg = message;
}

bool SpiceEngine::getErrorStatus(QString &message)
{
    if (errorFlag) message = errorMsg;
    return errorFlag;
}

int SpiceEngine::startSimulation(QString filename,
                                  const QMap<QString, BoundaryCondition *> *bcs,
                                  bool dump, QString dumpFilename)
{
    this->dump = dump;
    this->dumpFilename = dumpFilename;
    this->bcs = bcs;
    int ret = command("source " + filename);
    if (ret != 0) {
        setErrorFlag("NGSPICE: Error loading circuit file");
        return ret;
    }
    pthread_mutex_lock(&mutex);
    ret = run();
    if (ret != 0) {
        setErrorFlag("NGSPICE: Error running simulation");
        pthread_mutex_unlock(&mutex);
        return ret;
    }
    // wait for background thread to start
    while(no_bg) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    return 0;
}

int SpiceEngine::startSimulation(Netlist *netlist, bool dump, QString dumpFilename)
{
    this->dump = dump;
    this->dumpFilename = dumpFilename;
    filename = netlist->getFilename();
    this->netlist = netlist;
    int ret = command("source " + filename);
    if (ret != 0) {
        setErrorFlag("NGSPICE: Error loading circuit file");
        return ret;
    }
    pthread_mutex_lock(&mutex);
    ret = run();
    if (ret != 0) {
        setErrorFlag("NGSPICE: Error running simulation");
        pthread_mutex_unlock(&mutex);
        return ret;
    }
    // wait for background thread to start
    while(no_bg) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    return 0;
}

int SpiceEngine::resumeSimulation()
{
    int ret = resume();
    if (ret != 0) setErrorFlag("NGSPICE: Error resuming simulation");
    return ret;
}

int SpiceEngine::stopSimulation()
{
    if (no_bg) return 0;
    int ret = halt(); // note: halt will not return until no_bg == true
    if (ret != 0) setErrorFlag("NGSPICE: Error halting simulation");
    return ret;
}

int SpiceEngine::saveResults(QList<QString> vecs, bool bin, QString filename)
{
    QString command = "write " + filename + " ";
    foreach(QString vec, vecs) command += vec + " ";
    int ret;
    if (!bin) ret = this->command("set filetype=ascii");
    if (ret != 0) {
        setErrorFlag("NGSPICE: Error changing ngspice settings");
        return ret;
    }
    ret = this->command(command);
    if (ret != 0) {
        setErrorFlag("NGSPICE: Error saving vectors");
        return ret;
    }
    ret = this->command("set filetype=binary");
    if (ret != 0) {
        setErrorFlag("NGSPICE: Error changing ngspice settings");
        return ret;
    }

    return 0;
}

int SpiceEngine::plotResults(QList<QString> vecs, bool png, QString filename)
{
    QString command = "gnuplot " + filename + " ";
    foreach(QString vec, vecs) command += vec + " ";
    int ret;
    if (png) ret = this->command("set gnuplot_terminal=png");
    if (ret != 0) {
        setErrorFlag("NGSPICE: Error changing ngspice settings");
        return ret;
    }
    ret = this->command(command);
    if (ret != 0) {
        setErrorFlag("NGSPICE: Error plotting vectors");
        return ret;
    }
    if (png) ret = this->command("set gnuplot_terminal=eps");
    if (ret != 0) {
        setErrorFlag("NGSPICE: Error changing ngspice settings");
        return ret;
    }

    return 0;

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
    if (stat != 0.0) emit statusUpdate(static_cast<int>(stat));
}

void SpiceEngine::writeOutput(char *output)
{
    if (QString(output).startsWith("stderr Error:", Qt::CaseInsensitive))
        emit spiceError(QString(output));
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

QList<QString> SpiceEngine::vectors()
{
    QList<QString> vectorNames;
    foreach(pvecinfo v, vectorInfo) {
        vectorNames.append(v->vecname);
    }
    return vectorNames;
}

void SpiceEngine::setVecInfo(pvecinfoall info)
{
    plotName = QString(info->name);
    plotTitle = QString(info->title);
    plotDate = QString(info->date);
    plotType = QString(info->type);
    numVectors = info->veccount;
    vectorInfo.clear();
    for(int i = 0; i < numVectors; i++) {
        vectorInfo.append(info->vecs[i]);
    }
    emit initDataReady();
}

// ============= NGSPICE CALLBACK FUNCTIONS ======================

int initdata(pvecinfoall intdata, int ident, void* userdata)
{
    Q_UNUSED(ident);
    SpiceEngine *engine = static_cast<SpiceEngine *>(userdata);
    engine->setVecInfo(intdata);
    return 0;
}

/* Callback function called from bg thread in ngspice to transfer
any string created by printf or puts. Output to stdout in ngspice is
preceded by token stdout, same with stderr.*/
int getchar(char* outputreturn, int ident, void* userdata)
{
    Q_UNUSED(ident);
    SpiceEngine *engine = static_cast<SpiceEngine *>(userdata);
    engine->writeOutput(outputreturn);
    return 0;
}


int getstat(char* outputreturn, int ident, void* userdata)
{
    Q_UNUSED(ident);
    SpiceEngine *engine = static_cast<SpiceEngine *>(userdata);
    engine->emitStatusUpdate(outputreturn);
    return 0;
}

int thread_runs(bool noruns, int ident, void* userdata)
{
    Q_UNUSED(ident);
    SpiceEngine *engine = static_cast<SpiceEngine *>(userdata);
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
    SpiceEngine *engine = static_cast<SpiceEngine *>(userdata);
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
        ngSpice_Command(const_cast<char *>("quit 5"));
    }

    return exitstatus;
}

int getvoltage(double* voltage, double t, char* node, int ident, void* userdata)
{
    Q_UNUSED(ident);
    SpiceEngine *engine = static_cast<SpiceEngine *>(userdata);
    engine->getVoltage(voltage, t, node);
    return 0;
}

