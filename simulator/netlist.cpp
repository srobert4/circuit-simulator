#include "netlist.h"

/* Constructor */
Netlist::Netlist(const QString &name, QObject *parent) : QObject(parent)
{
    this->setName(name);
}

// ================== PUBLIC FUNCTIONS =========================================

/* Public Function: addElement(CircuitElement *, int, int)
 * -------------------------------------------------------
 * Add element using given CircuitElement object and nodes
 */
int Netlist::addElement(CircuitElement *element, int nodeIn, int nodeOut)
{
    if (element->getValue() == "" && element->getExternalFile() == "")
        return NoValueError;
    if (element->getName().length() == 1) return NoNameError;
    if (elementNames.contains(element->getName())) return DuplicateNameError;

    if (element->getName().front() == "V" && nodeIn == 0) {
        nodeIn = nodeOut;
        nodeOut = 0;
    }
    QString line = element->getName() + " " + QString::number(nodeIn) + " " +
            QString::number(nodeOut);
    if (element->getExternalFile() != "") {
        line += " external";
        BoundaryCondition *bc = new BoundaryCondition(element->getExternalFile(),
                                                      this);
        boundaryConditions[element->getName().toLower()] = bc;
    } else {
        line += (" " + element->getValue());
    }

    elementNames.insert(element->getName());
    nodeNames.insert(QString::number(nodeIn));
    nodeNames.insert(QString::number(nodeOut));
    elements.append(line);
    return 0;
}

/* Public Function: groundElement(CircuitElement *)
 * ------------------------------------------------
 * Connect the given element to ground (change second
 * node to 0)
 */
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

/* Public Function: writeToFile(const QString &)
 * ---------------------------------------------
 * Write netlist to file as a script that can be
 * run by ngspice.
 */
void Netlist::writeToFile(const QString &filename)
{
    QFile file(filename);
    if (file.open(QFile::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        // name
        out << name << endl;
        // elements
        for (QString elem : elements) {
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
