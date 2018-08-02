#ifndef SAVEWIZARDPAGE_H
#define SAVEWIZARDPAGE_H

#include <QtWidgets>

class SaveWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit SaveWizardPage(bool saveOnly, QWidget *parent = nullptr);
    QString getName() {return nameLineEdit->text();}
    QString getFilename() {return saveDirLineEdit->text() + "/" + filenameLineEdit->text() + ".cir";}

protected:
    virtual bool validatePage() override;
    virtual bool isComplete() const override;

private:
    QLineEdit *nameLineEdit;
    QLineEdit *saveDirLineEdit;
    QLineEdit *filenameLineEdit;

    QMessageBox *conf;


signals:
    void ready();

public slots:
};

#endif // SAVEWIZARDPAGE_H
