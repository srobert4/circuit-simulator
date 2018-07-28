#ifndef SIMOPTIONSWIZARDPAGE_H
#define SIMOPTIONSWIZARDPAGE_H

#include <QtWidgets>

class SimOptionsWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit SimOptionsWizardPage(QWidget *parent = nullptr);

protected:
//    virtual bool isComplete() const override;
    virtual bool validatePage() override;

private:
    QLineEdit *outputLine;

    QList<QString> simulationTypes = {
        "",
        ".tran"
    };
    QMap<int, QWidget *> simulationExtensions;
    int visibleExtension = 0;

    QWidget *createTranExtension();
    QList<QString> timeUnits = {
        "", "m", "u", "n"
    };

signals:

public slots:
};

#endif // SIMOPTIONSWIZARDPAGE_H
