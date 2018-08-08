#ifndef SAVEWIZARDPAGE_H
#define SAVEWIZARDPAGE_H

#include <QtWidgets>
#include "../simulation/netlist.h"

class SaveWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit SaveWizardPage(bool saveOnly, Netlist *netlist, QWidget *parent = nullptr);
    QString getName() {return nameLineEdit->text();}
    QString getFilename() {return saveDirLineEdit->text() + "/" + filenameLineEdit->text() + ".cir";}

protected:
    virtual bool validatePage() override;
    virtual bool isComplete() const override;
    virtual void initializePage() override;

private:
    QLabel *intro;
    QLineEdit *nameLineEdit;
    QLineEdit *saveDirLineEdit;
    QLineEdit *filenameLineEdit;

    bool saveOnly;
    Netlist *netlist;

    void processInput();
};

#endif // SAVEWIZARDPAGE_H
