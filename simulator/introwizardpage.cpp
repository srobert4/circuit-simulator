#include "introwizardpage.h"

/* Constructor */
IntroWizardPage::IntroWizardPage(QMap<QString, BoundaryCondition *> *bcMap,
                                 QWidget *parent) : QWizardPage(parent)
{
    this->bcMap = bcMap;
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
        emit completeChanged();
    });

    connect(fileLineEdit, &QLineEdit::textChanged, [this](){
        if(QFile(fileLineEdit->text()).exists()) {
            showExternalInputSelector();
        } else {
            hideExternalInputSelector();
        }
        emit completeChanged();
    });

    fileLoaderLayout->addWidget(fileLineEdit);
    fileLoaderLayout->addWidget(browseButton);
    fileLoader->setLayout(fileLoaderLayout);
    fileLoader->setHidden(true);

    // Connect buttons
    connect(parseButton, &QRadioButton::toggled, [=] () {
        if (!parseButton->isChecked()) return;
        setCommitPage(false);
        fileLoader->setHidden(true);
        hideExternalInputSelector();
        emit completeChanged();
    });

    connect(loadFileButton, &QRadioButton::toggled, [=](){
        if (!loadFileButton->isChecked()) return;
        setCommitPage(true);
        setButtonText(QWizard::CommitButton, "&Run >");
        fileLoader->setVisible(true);
        showExternalInputSelector();
        emit completeChanged();
    });

    // Set up layout
    layout = new QVBoxLayout;
    layout->addWidget(text);
    layout->addWidget(parseButton);
    layout->addWidget(loadFileButton);
    layout->addWidget(fileLoader);

    setLayout(layout);
}

void IntroWizardPage::showExternalInputSelector()
{
    // if no filename - return
    if (fileLineEdit->text().isEmpty()) return;
    // parse file for external elements
    QList<QString> external;
    QFile file(fileLineEdit->text());
    if (file.open(QFile::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.readLine(); // skip title line
        while(!in.atEnd()) {
            QString line = in.readLine();
            QStringList tokens = line.toLower().split(" ");
            if (tokens[0].startsWith("*")) continue; // skip comments
            if (tokens.contains("external")) external.append(tokens[0]);
        }
    }
    file.close();
    if (external.empty()) return;

    // show input selector
    inputSelector = new QWidget(this);
    QGridLayout *inputSelectorLayout = new QGridLayout;
    QLabel *desc = new QLabel("Select input files for external elements: ", this);
    inputSelectorLayout->addWidget(desc, 0, 0, 1, 3);
    for(int i = 0; i < external.length(); i++) {
        QString elem = external[i];
        inputSelectorLayout->addWidget(new QLabel(elem, this), i + 1, 0);

        QLineEdit *inputFileLineEdit = new QLineEdit(this);
        connect(inputFileLineEdit, &QLineEdit::textChanged, [=](){
            emit completeChanged();
        });
        selectedFiles[elem] = inputFileLineEdit;
        inputSelectorLayout->addWidget(inputFileLineEdit, i + 1, 1);

        QPushButton *browseButton = new QPushButton("Browse", this);
        connect(browseButton, &QPushButton::pressed,[=](){
            inputFileLineEdit->setText( QFileDialog::getOpenFileName(
                                       inputSelector,
                                       "Choose file",
                                       "/home/srobertson",
                                       "All files (*.*)"));
        });
        inputSelectorLayout->addWidget(browseButton, i + 1, 2);
    }
    inputSelector->setLayout(inputSelectorLayout);
    layout->addWidget(inputSelector);
}

void IntroWizardPage::hideExternalInputSelector()
{
    if (!inputSelector) return;
    selectedFiles.clear();
    layout->removeWidget(inputSelector);
    delete inputSelector;
    inputSelector = nullptr;
}

void IntroWizardPage::populateBoundaryConditions()
{
    foreach(QString node, selectedFiles.keys()){
        bcMap->insert(node, new BoundaryCondition(selectedFiles[node]->text()));
    }
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
    if (fileLineEdit->text().isEmpty()) return false;
    if (!(QFileInfo::exists(fileLineEdit->text()) &&
            QFileInfo(fileLineEdit->text()).isFile())) return false;
    foreach(QLineEdit *line, selectedFiles) {
        if (line->text().isEmpty()) return false;
        if (!(QFileInfo::exists(line->text()) &&
              QFileInfo(line->text()).isFile())) return false;
    }
    return true;
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

    foreach(QLineEdit *line, selectedFiles) {
        if (line->text().isEmpty()) return false;
        if (!BoundaryCondition::checkFile(line->text())){
            QMessageBox::critical(this,
                                  "Bad Input File",
                                  "One or more input files provided "
                                  "is not correctly formatted. Format should"
                                  " be: <time>\\t<value>");
            return false;
        }
    }

    if (QMessageBox::question(this,
                                 "Start simulation?",
                                 "Continue?\nYou will not be able to "
                                 "return to this page",
                                 (QMessageBox::Cancel | QMessageBox::Yes))
            == QMessageBox::Cancel) return false;

    populateBoundaryConditions();
    return true;
}
