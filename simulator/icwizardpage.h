#ifndef ICWIZARDPAGE_H
#define ICWIZARDPAGE_H

#include <QtWidgets>
#include "netlist.h"

/* Class: ICWizardPage
 * -------------------
 * Allows users to set initial conditions
 * for any nodes on the circuit schematic
 */
class ICWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit ICWizardPage(Netlist *netlist, QWidget *parent = nullptr);

protected:
    virtual bool isComplete() const override;
    virtual bool validatePage() override;
    virtual void initializePage() override;

private:
    Netlist *netlist;
    QLineEdit *outputLine;
    QVBoxLayout *layout;

signals:

public slots:
};

#endif // ICWIZARDPAGE_H
