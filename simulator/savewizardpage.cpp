#include "savewizardpage.h"

SaveWizardPage::SaveWizardPage(bool saveOnly, Netlist *netlist, QWidget *parent) : QWizardPage(parent)
{
    this->saveOnly = saveOnly;
    this->netlist = netlist;

    // Page set-up
    setCommitPage(true);
    setTitle("Save Your Circuit");
    QLabel *label = new QLabel("Save your circuit to a file so that you can run"
                               " this simulation again without drawing a new schematic. "
                               "Once you save your circuit, you cannot edit the circuit or"
                               " simulation settings further in this wizard.", this);
    label->setWordWrap(true);    
    setButtonText(QWizard::CommitButton, "&Run >");
    if (saveOnly) {
        setButtonText(QWizard::FinishButton, "Save & Exit");
    } else {
        setButtonText(QWizard::FinishButton, "Save & Start Simulation");
    }

    // Circuit Name
    nameLineEdit = new QLineEdit(this);
    registerField("circuitName*", nameLineEdit);

    // Filename & directory browser
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
    setLayout(layout);
}

// ===================== PRIVATE FUNCTIONS =====================================

/* Private Function: saveNetlist()
 * -------------------------------
 * Saves netlist to file with all settings
 * given in the wizard. Called when the user
 * clicks next.
 */
void SaveWizardPage::saveNetlist()
{
    netlist->setName(field("circuitName").toString());
    netlist->setAnalysis(field("simOptions").toString());
    netlist->setInitialConditions(field("initialConditions").toString());
    netlist->writeToFile(getFilename());
}

// ====================== PROTECTED FUNCTIONS ==================================

/* Protected Function: validatePage()
 * ---------------------------------
 * Gets confirmation from the user that
 * they're ready to save and continue to
 * the simulation page. If yes, writes
 * the netlist to file.
 */
bool SaveWizardPage::validatePage() {
    if (saveDirLineEdit->text() == "" ||
            filenameLineEdit->text() == "") return false;
    if (!QDir(saveDirLineEdit->text()).exists()) return false;
    if (!saveOnly && QMessageBox::question(this,
                                 "Start simulation?",
                                 "Continue?\nYou will not be able to return to this page",
                                 (QMessageBox::Cancel | QMessageBox::Yes)) == QMessageBox::Cancel) return false;
    saveNetlist();
    return true;
}

/* Protected Function: isComplete()
 * -------------------------------
 * Only enable next button if a valid filename
 * is given.
 */
bool SaveWizardPage::isComplete() const {
    if (saveDirLineEdit->text() == "" ||
            filenameLineEdit->text() == "") return false;
    return QDir(saveDirLineEdit->text()).exists();
}
