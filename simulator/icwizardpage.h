#ifndef ICWIZARDPAGE_H
#define ICWIZARDPAGE_H

#include <QtWidgets>

class ICWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit ICWizardPage(QSet<QString> nodeNames, QWidget *parent = nullptr);

protected:
    virtual bool validatePage() override;

private:
    QLineEdit *outputLine;
    QSet<QString> nodes;

signals:

public slots:
};

#endif // ICWIZARDPAGE_H
