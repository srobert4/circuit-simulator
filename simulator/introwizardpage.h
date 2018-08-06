#ifndef INTROWIZARDPAGE_H
#define INTROWIZARDPAGE_H

#include "boundarycondition.h"
#include <QtWidgets>

class IntroWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit IntroWizardPage(QMap<QString, BoundaryCondition *> *bcMap,
                             QWidget *parent = nullptr);

signals:
    void parseCircuit();

protected:
    virtual bool isComplete() const override;
    virtual bool validatePage() override;

private:
    QVBoxLayout *layout;
    QRadioButton *parseButton;
    QRadioButton *loadFileButton;
    QLineEdit *fileLineEdit;

    QMessageBox *conf;
    bool parseComplete = false;

    void showExternalInputSelector();
    void hideExternalInputSelector();
    QMap<QString, BoundaryCondition *> *bcMap;
    QWidget *inputSelector = nullptr;
    QMap<QString, QLineEdit *> selectedFiles;
    void populateBoundaryConditions();

};

#endif // INTROWIZARDPAGE_H
