#ifndef SIMULATEWIZARDPAGE_H
#define SIMULATEWIZARDPAGE_H

#include <QtWidgets>

class simulateWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit simulateWizardPage(QWidget *parent = nullptr);
protected:
    virtual void initializePage();
private:
    QVBoxLayout *layout;
signals:

public slots:
};

#endif // SIMULATEWIZARDPAGE_H
