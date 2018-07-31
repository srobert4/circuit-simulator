#ifndef SIMOPTIONSWIZARDPAGE_H
#define SIMOPTIONSWIZARDPAGE_H

#include <QtWidgets>

class SimOptionsWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit SimOptionsWizardPage(QGraphicsScene *schem, QWidget *parent = nullptr);

protected:
//    virtual bool isComplete() const override;
    virtual bool validatePage() override;
    virtual void initializePage() override;

private:
    QGraphicsScene *schematic;
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
    void parseSchematic();

public slots:
};

#endif // SIMOPTIONSWIZARDPAGE_H
