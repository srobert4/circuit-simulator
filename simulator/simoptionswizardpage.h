#ifndef SIMOPTIONSWIZARDPAGE_H
#define SIMOPTIONSWIZARDPAGE_H

#include <QtWidgets>
#include "netlist.h"

class SimOptionsWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit SimOptionsWizardPage(QGraphicsScene *schem, Netlist *netlist, QWidget *parent = nullptr);

protected:
    virtual bool isComplete() const override;
    virtual bool validatePage() override;
    virtual void initializePage() override;

private:
    QGraphicsScene *schematic;
    Netlist *netlist;
    QFormLayout *layout;
    QLineEdit *outputLine;

    QList<QString> simulationTypes = {
        "",
        ".tran",
        ".dc"
    };
    QMap<int, QWidget *> simulationExtensions;
    int visibleExtension = 0;

    QWidget *createTranExtension();
    QList<QString> timeUnits = {
        "", "m", "u", "n"
    };

    QWidget *createDCExtension(QSet<QString> elements);

signals:
    void parseSchematic();

public slots:
};

#endif // SIMOPTIONSWIZARDPAGE_H
