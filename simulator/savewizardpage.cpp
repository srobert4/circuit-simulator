#include "savewizardpage.h"

SaveWizardPage::SaveWizardPage(bool saveOnly, QWidget *parent) : QWizardPage(parent)
{
    setTitle("Save Your Circuit");

    QLabel *label = new QLabel("Save your circuit to a file so that you can run"
                               " this simulation again without drawing a new schematic.", this);
    label->setWordWrap(true);

    nameLineEdit = new QLineEdit(this);
    registerField("circuitName*", nameLineEdit);

    QWidget *browser = new QWidget(this);
    saveDirLineEdit = new QLineEdit(this);
    registerField("saveDir*", saveDirLineEdit);
    QPushButton *browseButton = new QPushButton("Browse", this);
    QHBoxLayout *browserLayout = new QHBoxLayout;

    connect(browseButton, &QPushButton::pressed,
            [=](){ saveDirLineEdit->setText( QFileDialog::getExistingDirectory(
                                                 browser,
                                                 "Choose save directory",
                                                 "/home/srobertson",
                                                 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                                                 )); });

    browserLayout->addWidget(saveDirLineEdit);
    browserLayout->addWidget(browseButton);
    browser->setLayout(browserLayout);

    QWidget *filename = new QWidget(this);
    filenameLineEdit = new QLineEdit(this);
    registerField("saveFilename*", filenameLineEdit);
    QLabel *ext = new QLabel(".cir", this);
    QHBoxLayout *filenameLayout = new QHBoxLayout;
    filenameLayout->addWidget(filenameLineEdit);
    filenameLayout->addWidget(ext);
    filename->setLayout(filenameLayout);

    QFormLayout *layout = new QFormLayout;
    layout->addRow(label);
    layout->addRow("Model name: ", nameLineEdit);
    layout->addRow("Choose directory to save circuit in: ", browser);
    layout->addRow("Save circuit as: ", filename);
    layout->setRowWrapPolicy(QFormLayout::WrapAllRows);

    connect(browseButton, &QPushButton::pressed, [this](){ emit completeChanged(); });
    connect(nameLineEdit, &QLineEdit::textEdited, [this](){ emit completeChanged(); });
    connect(filenameLineEdit, &QLineEdit::textEdited, [this](){ emit completeChanged(); });
    connect(saveDirLineEdit, &QLineEdit::textEdited, [this](){ emit completeChanged(); });

    if (saveOnly) setButtonText(QWizard::FinishButton, "Save & Exit");
    setLayout(layout);
}

bool SaveWizardPage::validatePage() {
    if (saveDirLineEdit->text() == "" ||
            filenameLineEdit->text() == "") return false;
    if (!QDir(saveDirLineEdit->text()).exists()) return false;
    emit ready();
    return true;
}

bool SaveWizardPage::isComplete() const {
    if (saveDirLineEdit->text() == "" ||
            filenameLineEdit->text() == "") return false;
    return QDir(saveDirLineEdit->text()).exists();
}
