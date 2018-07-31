#ifndef ICWIZARDPAGE_H
#define ICWIZARDPAGE_H

#include <QtWidgets>
#include "netlist.h"

class ICWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit ICWizardPage(Netlist *netlist, QWidget *parent = nullptr);

protected:
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
