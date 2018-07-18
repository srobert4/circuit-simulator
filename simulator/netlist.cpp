
#include "netlist.h"

Netlist::Netlist(const QString &name, QObject *parent) : QObject(parent)
{
    this->setName(name);
}

void Netlist::setName(const QString &name) { this->name = name; }

void Netlist::addElement(
        const QString &element,
        const QString &name,
        const QString &nodeIn,
        const QString &nodeOut,
        const QString &value,
        const QString &units,
        const QString &externalFilename
        )
{
    // construct element
    QString line = element + name + " "  + nodeIn + " " + nodeOut + " " + value + units;

    // add boundary conditions if necessary
    if (externalFilename != "") {
        boundaryConditions[element + name] = externalFilename;
    }

    elementNames.insert(element + name);
    nodeNames.insert(nodeIn);
    nodeNames.insert(nodeOut);
    elements.append(line);
}

void Netlist::addElement(CircuitElement *element, int nodeIn, int nodeOut)
{
    QString line = element->getName() + " " + QString::number(nodeIn) + " " + QString::number(nodeOut);
    if (element->getExternalFile() != "") {
        line += " external";
        boundaryConditions[element->getName()] = element->getExternalFile();
    } else {
        line += (" " + element->getValue());
    }
    elementNames.insert(element->getName());
    nodeNames.insert(QString::number(nodeIn));
    nodeNames.insert(QString::number(nodeOut));
    elements.append(line);
}

void Netlist::addInitialCondition(QString node, const QString &condition)
{
    bool ok;
    node.toInt(&ok);
    node = (ok ? "v(" + node + ")" : node);

    initialConditions.append(".ic " + node + "=" + condition);
}

void Netlist::setGraphing(QString &graphName, QList<QString> &nodes)
{
    graphingCommand = "gnuplot " + graphName;
    for (QString node : nodes)
        graphingCommand += (" " + node);
}

// TODO: Make this flexible for different analysis lines (.tran .dc)
void Netlist::setAnalysis(
    const QString &type,
    const QString &step,
    const QString &stepUnits,
    const QString &time,
    const QString &timeUnits
)
{
    analysis = type + " " + step + stepUnits + " " + time + timeUnits;
}


void Netlist::writeToFile(const QString &filename)
{
    QFile file(filename);
    if (file.open(QFile::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        // name
        out << name << endl;
        // elements
        for (QString elem : elements) {
            qInfo() << elem;
            out << elem << endl;
        }
        // ics
        for (QString ic : initialConditions)
            out << ic << endl;
        // analysis
        out << analysis << endl;
        // end
        out << ".end" << endl;
        file.close();
    }
    this->filename = filename;
}

QString Netlist::getCommand()
{
    QString command =  "./main " + filename + " ";
    for (QString node : boundaryConditions)
        command += (node + "=" + boundaryConditions[node]);
    command += "\"" + graphingCommand + "\"";
    return command;
}
