#ifndef SIMOPTIONSWIZARDPAGE_H
#define SIMOPTIONSWIZARDPAGE_H

#include <QtWidgets>
#include "netlist.h"

class SimOptionsWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit SimOptionsWizardPage(QGraphicsScene *schem, Netlist *netlist, bool saveOnly, QWidget *parent = nullptr);

protected:
    virtual bool isComplete() const override;
    virtual bool validatePage() override;
    virtual void initializePage() override;

private:
    QGraphicsScene *schematic;
    Netlist *netlist;
    QFormLayout *layout;
    QLineEdit *outputLine;
    bool saveOnly;

    QMap<QString, QString> simulationModes = {
        {"Transient", ".tran"},
        {"DC", ".dc"}
    };
    QList<QString> modes; // holds order of combobox
    QMap<int, QWidget *> simulationExtensions;
    int visibleExtension = 0;
    QString mu;
    QList<QString> unitModifiers;

    QWidget *createTranExtension();
    QWidget *createDCExtension(QSet<QString> elements);

signals:
    void parseSchematic();

public slots:
};

#endif // SIMOPTIONSWIZARDPAGE_H
