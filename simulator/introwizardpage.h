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
    virtual bool validatePage() override;

private:
    QRadioButton *parseButton;
    QRadioButton *loadFileButton;
    QLineEdit *fileLineEdit;
    QString filename;

    QMessageBox *conf;
};

#endif // INTROWIZARDPAGE_H
