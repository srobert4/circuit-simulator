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

    int addElement(CircuitElement *element, int nodeIn, int nodeOut);
    int groundElement(CircuitElement *element);

    void setName(const QString &name) { this->name = name; }
    void setFilename(const QString &filename) { this->filename = filename; }
    void setAnalysis(QString line) { analysis = line; }
    void setInitialConditions(QString line) { initialConditions.append(line); }

    void writeToFile(const QString &filename);
    QSet<QString> getElementNames() { return elementNames; }
    QSet<QString> getNodeNames() { return nodeNames; }
    double getBoundaryPressure(char *node, double time)
    {
        return boundaryConditions[node]->getState(time);
    }
    const QMap<QString, BoundaryCondition *> *getBoundaryConditions()
    {
        return &boundaryConditions;
    }
    QString getFilename() { return filename; }
    QMap<QString, BoundaryCondition *> boundaryConditions;

private:
    QString name;
    QVector<QString> elements;
    QSet<QString> elementNames, nodeNames;
    QVector<QString> initialConditions;
    QString analysis;
    QString filename;
    QString graphingCommand;
};

#endif // NETLIST_H
