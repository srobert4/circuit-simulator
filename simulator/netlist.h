#ifndef NETLIST_H
#define NETLIST_H

#include <QtWidgets>
#include "circuitelement.h"
#include "boundarycondition.h"

class Netlist : public QObject
{
    Q_OBJECT
public:
    explicit Netlist(const QString &name = "", QObject *parent = nullptr);
    ~Netlist() {}

    enum { NoValueError = 3, NoNameError = 4, DuplicateNameError = 5 };

    int addElement(
            const QString &element,
            const QString &name,
            const QString &nodeIn,
            const QString &nodeOut,
            const QString &value,
            const QString &units = "",
            const QString &externalFilename = ""
            );
    int addElement(CircuitElement *element, int nodeIn, int nodeOut);
    int groundElement(CircuitElement *element);
    void addInitialCondition(QString node, const QString &condition);

    void setName(const QString &name);
    void setFilename(const QString &filename);
    void setGraphing(QString &graphName, QList<QString> &nodes);
    void setAnalysis(
        const QString &type,
        const QString &step,
        const QString &stepUnits,
        const QString &time,
        const QString &timeUnits
    );
    void setAnalysis(QString line) { analysis = line; }
    void setInitialConditions(QString line) { initialConditions.append(line); }

    void writeToFile(const QString &filename);
    QSet<QString> getElementNames() { return elementNames; }
    QSet<QString> getNodeNames() { return nodeNames; }
    double getBoundaryPressure(char *node, double time) { return boundaryConditions[node]->getState(time); }
    const QMap<QString, BoundaryCondition *> *getBoundaryConditions() { return &boundaryConditions; }
    QString getFilename() { return filename; }
    bool ready() {return fileReady;}
    QMap<QString, BoundaryCondition *> boundaryConditions;

private:
    QString name;
    QVector<QString> elements;
    QSet<QString> elementNames, nodeNames;
    QVector<QString> initialConditions;
    QString analysis;
    QString filename;
    QString graphingCommand;
    bool fileReady;
};

#endif // NETLIST_H
