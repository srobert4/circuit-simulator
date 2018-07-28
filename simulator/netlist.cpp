
#include "netlist.h"

Netlist::Netlist(const QString &name, QObject *parent) : QObject(parent)
{
    this->setName(name);
    fileReady = false;
}

void Netlist::setName(const QString &name) { this->name = name; }
void Netlist::setFilename(const QString &filename) {
    this->filename = filename;
    fileReady = true;
}

int Netlist::addElement(
        const QString &element,
        const QString &name,
        const QString &nodeIn,
        const QString &nodeOut,
        const QString &value,
        const QString &units,
        const QString &externalFilename
        )
{
    if (name == "") return NoNameError;
    if (value == "") return NoValueError;
    if (elementNames.contains(element + name)) return DuplicateNameError;

    // construct element
    QString line = element + name + " "  + nodeIn + " " + nodeOut + " " + value + units;

    // add boundary conditions if necessary
    if (externalFilename != "") {
        BoundaryCondition *bc = new BoundaryCondition(externalFilename, 1.0, this);
        boundaryConditions[element + name] = bc;
    }
    elementNames.insert(element + name);
    nodeNames.insert(nodeIn);
    nodeNames.insert(nodeOut);
    elements.append(line);
    return 0;
}

int Netlist::addElement(CircuitElement *element, int nodeIn, int nodeOut)
{
    if (element->getValue() == "" and element->getExternalFile() == "") return NoValueError;
    if (element->getName().length() == 1) return NoNameError;
    if (elementNames.contains(element->getName())) return DuplicateNameError;

    QString line = element->getName() + " " + QString::number(nodeIn) + " " + QString::number(nodeOut);
    if (element->getExternalFile() != "") {
        line += " external";
        BoundaryCondition *bc = new BoundaryCondition(element->getExternalFile(), element->getPeriod(), this);
        boundaryConditions[element->getName()] = bc;
    } else {
        line += (" " + element->getValue());
    }

    elementNames.insert(element->getName());
    nodeNames.insert(QString::number(nodeIn));
    nodeNames.insert(QString::number(nodeOut));
    elements.append(line);
    return 0;
}

int Netlist::groundElement(CircuitElement *element)
{
    if (!elementNames.contains(element->getName())) return -1;
    for (QString line : elements) {
        if (line.split(" ")[0] != element->getName()) continue;
        elements.removeOne(line);
        QStringList tokens = line.split(" ");
        nodeNames.remove(tokens[2]);
        tokens[2] = QString::number(0);
        elements.append(tokens.join(" "));
    }
    return 0;
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
    fileReady = true;
}
