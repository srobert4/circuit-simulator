#include "introwizardpage.h"

IntroWizardPage::IntroWizardPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle("Circuit Simulation Wizard");
    setCommitPage(true);
    setButtonText(QWizard::CommitButton, "Next");

    QLabel *text = new QLabel("Welcome to the circuit simulation wizard. Would you "
                              "like to use the circuit you have drawn or load a "
                              "previously saved circuit?", this);
    text->setWordWrap(true);

    QButtonGroup *answer = new QButtonGroup(this);
    parseButton = new QRadioButton("Use this circuit", this);
    loadFileButton = new QRadioButton("Load circuit from file", this);
    registerField("parseCircuit", parseButton);
    registerField("loadCircuit", loadFileButton);
    answer->addButton(parseButton);
    answer->addButton(loadFileButton);
    answer->setExclusive(true);

    connect(parseButton, &QRadioButton::toggled, [this] () { emit completeChanged(); });
    connect(loadFileButton, &QRadioButton::toggled, [this](){ emit completeChanged(); });

    QWidget *fileLoader = new QWidget(this);
    fileLineEdit = new QLineEdit(this);
    registerField("filename", fileLineEdit);
    QPushButton *browseButton = new QPushButton("Browse", this);
    QHBoxLayout *fileLoaderLayout = new QHBoxLayout;

    connect(browseButton, &QPushButton::pressed,
            [=](){
        fileLineEdit->setText( QFileDialog::getOpenFileName(
                                   fileLoader,
                                   "Choose file",
                                   "/home/srobertson",
                                   "All files (*.cir)"));
        emit completeChanged();}
    );

    connect(fileLineEdit, &QLineEdit::textEdited, [this](){ emit completeChanged(); });

    fileLoaderLayout->addWidget(fileLineEdit);
    fileLoaderLayout->addWidget(browseButton);
    fileLoader->setLayout(fileLoaderLayout);
    fileLoader->setHidden(true);

    connect(parseButton, &QRadioButton::toggled, fileLoader, &QWidget::setHidden);
    connect(loadFileButton, &QRadioButton::toggled, fileLoader, &QWidget::setVisible);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(text);
    layout->addWidget(parseButton);
    layout->addWidget(loadFileButton);
    layout->addWidget(fileLoader);

    setLayout(layout);

    conf = new QMessageBox;
    conf->setIcon(QMessageBox::Question);
    conf->setText("Start simulation?");
    conf->setInformativeText("Once you start the simulation, you will not be able to "
                             "edit the circuit selected.");
    conf->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    conf->setDefaultButton(QMessageBox::No);
    conf->setButtonText(QMessageBox::Yes, "Start simulation");
    conf->setButtonText(QMessageBox::No, "Return");
}

bool IntroWizardPage::isComplete() const {
    if (parseButton->isChecked()) return true;
    if (!loadFileButton->isChecked()) return false;
    if (fileLineEdit->text() == "") return false;
    return QFileInfo::exists(fileLineEdit->text()) && QFileInfo(fileLineEdit->text()).isFile();
}

bool IntroWizardPage::validatePage() {
    if (field("parseCircuit").toBool()) return true;
    return (conf->exec() == QMessageBox::Yes);
}
