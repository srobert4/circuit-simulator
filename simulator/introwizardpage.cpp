#include "introwizardpage.h"

/* Constructor */
IntroWizardPage::IntroWizardPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle("Circuit Simulation Wizard");
    QLabel *text = new QLabel("Welcome to the circuit simulation wizard. Would you "
                              "like to use the circuit you have drawn or load a "
                              "previously saved circuit?", this);
    text->setWordWrap(true);

    // Create buttons
    QButtonGroup *answer = new QButtonGroup(this);
    parseButton = new QRadioButton("Use this circuit", this);
    loadFileButton = new QRadioButton("Load circuit from file", this);
    registerField("parseCircuit", parseButton);
    registerField("loadCircuit", loadFileButton);
    answer->addButton(parseButton);
    answer->addButton(loadFileButton);
    answer->setExclusive(true);


    // File browser widget
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

    // Connect buttons
    connect(parseButton, &QRadioButton::toggled, [=] () {
        setCommitPage(false);
        fileLoader->setHidden(true);
        emit completeChanged();
    });

    connect(loadFileButton, &QRadioButton::toggled, [=](){
        setCommitPage(true);
        setButtonText(QWizard::CommitButton, "&Run >");
        fileLoader->setVisible(true);
        emit completeChanged();
    });

    // Set up layout
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(text);
    layout->addWidget(parseButton);
    layout->addWidget(loadFileButton);
    layout->addWidget(fileLoader);

    setLayout(layout);
}

// =========== PROTECTED FUNCTIONS ======================

/* isComplete()
 * ------------
 * Activates next button only if the parse option
 * is selected or if the file to load is valid.
 */
bool IntroWizardPage::isComplete() const {
    if (parseButton->isChecked()) return true;
    if (!loadFileButton->isChecked()) return false;
    if (fileLineEdit->text() == "") return false;
    return QFileInfo::exists(fileLineEdit->text()) && QFileInfo(fileLineEdit->text()).isFile();
}

/* validatePage()
 * --------------
 * If parse option selected - parse the circuit
 * Else - confirm that the user wants to go to sim page with no return option
 */
bool IntroWizardPage::validatePage() {
    if (parseButton->isChecked()) {
        if (parseComplete) return true;
        emit parseCircuit();
        QApplication::processEvents();
        parseComplete = true;
        return true;
    }
    return QMessageBox::question(this,
                                 "Start simulation?",
                                 "Continue?\nYou will not be able to return to this page",
                                 (QMessageBox::Cancel | QMessageBox::Yes)) == QMessageBox::Yes;
}
