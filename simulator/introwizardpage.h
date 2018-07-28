#ifndef INTROWIZARDPAGE_H
#define INTROWIZARDPAGE_H

#include <QtWidgets>

class IntroWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit IntroWizardPage(QWidget *parent = nullptr);

signals:
    void filenameReady();

protected:
    virtual bool isComplete() const override;

private:
    QRadioButton *parseButton;
    QRadioButton *loadFileButton;
    QLineEdit *fileLineEdit;
    QString filename;
};

#endif // INTROWIZARDPAGE_H
