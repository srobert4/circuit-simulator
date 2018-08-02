#include "simulatewizardpage.h"

SimulateWizardPage::SimulateWizardPage(SpiceEngine *engine, Netlist *netlist, QWidget *parent) : QWizardPage(parent)
{
    setTitle("Run your simulation");
    this->netlist = netlist;
    this->engine = engine;

    QWidget *runWidget = new QWidget(this);
    QPushButton *runButton = new QPushButton("Click to Start Simulation", this);
    QCheckBox *dumpOutputCheckbox = new QCheckBox("Write simulation output to file?", this);
    registerField("dumpOutput", dumpOutputCheckbox);
    dumpFilenameLineEdit = new QLineEdit(this);
    dumpFilenameLineEdit->setHidden(true);
    registerField("dumpFilename", dumpFilenameLineEdit);
    QVBoxLayout *runLayout = new QVBoxLayout;
    runLayout->addWidget(runButton);
    runLayout->addWidget(dumpOutputCheckbox);
    runLayout->addWidget(dumpFilenameLineEdit);
    runWidget->setLayout(runLayout);

    QWidget *progressWidget = new QWidget(this);
    progressBar = new QProgressBar(this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    QHBoxLayout *progressLayout = new QHBoxLayout;
    progressLayout->addWidget(progressBar);
    progressBar->setRange(0, 100);
    progressLayout->addWidget(cancelButton);
    progressWidget->setLayout(progressLayout);
    progressWidget->setHidden(true);

    resultsLabel = new QLabel(this);

    connect(runButton, &QPushButton::pressed, [=](){
        runWidget->setHidden(true);
        progressWidget->setVisible(true);
        runSimulation();
    });
    connect(cancelButton, &QPushButton::pressed, [=](){
        runWidget->setVisible(true);
        progressWidget->setHidden(true);
        cancelSimulation();
    });
    connect(dumpOutputCheckbox, &QCheckBox::stateChanged, [=](){
        if (dumpOutputCheckbox->isChecked()) {
            dumpFilenameLineEdit->setVisible(true);
        } else {
            dumpFilenameLineEdit->setHidden(true);
        }
    });

    connect(engine, &SpiceEngine::statusUpdate,
            this, &SimulateWizardPage::updateStatus,
            Qt::QueuedConnection);

    connect(engine, &SpiceEngine::spiceError,
            this, &SimulateWizardPage::receiveError,
            Qt::QueuedConnection);

    connect(engine, &SpiceEngine::initDataReady,
            this, &SimulateWizardPage::initData,
            Qt::QueuedConnection);

    layout = new QVBoxLayout;
    layout->addWidget(runWidget);
    layout->addWidget(progressWidget);
    layout->addWidget(resultsLabel);
    setLayout(layout);
}

void SimulateWizardPage::runSimulation()
{
    running = true;
    if (field("loadCircuit").toBool()) {
        // TODO: allow user to provide filename for any External input elements
        // create bcs map, and pass a pointer to this function.
        engine->startSimulation(field("filename").toString(), nullptr, field("dumpOutput").toBool(), field("dumpFilename").toString());
    } else {
        engine->startSimulation(netlist, field("dumpOutput").toBool(), field("dumpFilename").toString());
    }
}

void SimulateWizardPage::cancelSimulation()
{
    engine->stopSimulation();
    running = false;
}

void SimulateWizardPage::showResults()
{
    plotButton->setEnabled(true);
    saveButton->setEnabled(true);
}

QWidget *SimulateWizardPage::getPlotWidget()
{
    QWidget *plotWidget = new QWidget;
    QVBoxLayout *plotLayout = new QVBoxLayout;

    // save question
    QHBoxLayout *saveLayout = new QHBoxLayout;
    saveLayout->addWidget(new QLabel("Save plot? ", this));
    QButtonGroup *saveButtons = new QButtonGroup(this);
    QRadioButton *yesButton = new QRadioButton("Yes", this);
    yesButton->setChecked(true);
    registerField("savePlot", yesButton);
    saveButtons->addButton(yesButton);
    saveLayout->addWidget(yesButton);
    QRadioButton *noButton = new QRadioButton("No", this);
    saveButtons->addButton(noButton);
    saveLayout->addWidget(noButton);
    saveLayout->addStretch(1);


    // save options
    QWidget *saveOptionsWidget = new QWidget(plotWidget);
    QVBoxLayout *saveOptionsLayout = new QVBoxLayout;
    connect(yesButton, &QRadioButton::toggled, saveOptionsWidget, &QWidget::setVisible);
    connect(noButton, &QRadioButton::toggled, saveOptionsWidget, &QWidget::setHidden);

    // file name
    QHBoxLayout *filenameLayout = new QHBoxLayout;
    filenameLayout->addWidget(new QLabel("Save as:", this));
    QLineEdit *filenameLineEdit = new QLineEdit(defaultFilename, this);
    registerField("plotFilename", filenameLineEdit);
    QPushButton *browseButton = new QPushButton("Browse", this);
    filenameLayout->addWidget(filenameLineEdit, 1);
    filenameLayout->addWidget(browseButton);

    connect(browseButton, &QPushButton::pressed,
            [=](){ filenameLineEdit->setText( QFileDialog::getOpenFileName(
                                                 saveOptionsWidget,
                                                 "Choose file",
                                                 "/home/srobertson",
                                                  "All files")); });
    // file type
    QHBoxLayout *formatLayout = new QHBoxLayout;
    formatLayout->addWidget(new QLabel("Format:"));
    QComboBox *plotFormat = new QComboBox(this);
    plotFormat->addItems({"PNG", "Postscript"});
    registerField("plotFormat", plotFormat, "currentText");
    formatLayout->addWidget(plotFormat);
    formatLayout->addStretch(1);

    saveOptionsLayout->addLayout(filenameLayout);
    saveOptionsLayout->addLayout(formatLayout);
    saveOptionsWidget->setLayout(saveOptionsLayout);

    // plot button
    plotButton = new QPushButton("Plot", this);
    plotButton->setEnabled(false);
    connect(plotButton, &QPushButton::released, this, &SimulateWizardPage::plot);

    // overall layout
    plotLayout->addLayout(saveLayout);
    plotLayout->addWidget(saveOptionsWidget);
    plotLayout->addWidget(plotButton, 0, Qt::AlignCenter);

    plotWidget->setLayout(plotLayout);
    return plotWidget;
}

QWidget *SimulateWizardPage::getSaveWidget()
{
    QWidget *saveWidget = new QWidget;
    QVBoxLayout *saveLayout = new QVBoxLayout;

    // filename
    QHBoxLayout *filenameLayout = new QHBoxLayout;
    filenameLayout->addWidget(new QLabel("Save as:", this));
    QLineEdit *filenameLineEdit = new QLineEdit(defaultFilename + "_out.raw", this);
    registerField("saveResultsFilename", filenameLineEdit);
    QPushButton *browseButton = new QPushButton("Browse", this);
    filenameLayout->addWidget(filenameLineEdit, 1);
    filenameLayout->addWidget(browseButton);

    connect(browseButton, &QPushButton::pressed,
            [=](){ filenameLineEdit->setText( QFileDialog::getOpenFileName(
                                                 saveWidget,
                                                 "Choose file",
                                                 "/home/srobertson",
                                                  "All files")); });
    // format
    QHBoxLayout *formatLayout = new QHBoxLayout;
    formatLayout->addWidget(new QLabel("Format:"));
    QComboBox *saveFormat = new QComboBox(this);
    saveFormat->addItems({"Compact Binary", "ASCII"});
    registerField("saveFormat", saveFormat, "currentText");
    formatLayout->addWidget(saveFormat);
    formatLayout->addStretch(1);

    // save button
    saveButton = new QPushButton("Save", this);
    saveButton->setEnabled(false);
    connect(saveButton, &QPushButton::released, this, &SimulateWizardPage::save);

    // layout
    saveLayout->addLayout(filenameLayout);
    saveLayout->addLayout(formatLayout);
    saveLayout->addWidget(saveButton, 0, Qt::AlignCenter);
    saveWidget->setLayout(saveLayout);
    return saveWidget;
}

void SimulateWizardPage::save()
{
    QList<QString> toWrite;
    for (int i = 0; i < selectedVectors.size(); i++) {
        if (selectedVectors.values()[i])
            toWrite.append(selectedVectors.keys()[i]);
    }
    if (toWrite.empty()) {
        int ret = QMessageBox::question(this, "Save all?", "No vectors selected. Save all vectors?",
                                        (QMessageBox::Yes | QMessageBox::Cancel));
        if (ret == QMessageBox::Cancel) return;
    }

    saveButton->setText("Saving...");
    update();
    QApplication::processEvents();
    engine->saveResults(toWrite, field("saveFormat").toString() == "Compact Binary", field("saveResultsFilename").toString());
    QMessageBox::information(this, "Save successful", "Your results have been saved!");

    saveButton->setText("Save");
}

void SimulateWizardPage::plot()
{
    QList<QString> toPlot;
    for (int i = 0; i < selectedVectors.size(); i++) {
        if (selectedVectors.values()[i])
            toPlot.append(selectedVectors.keys()[i]);
    }
    if (toPlot.empty()) {
        QMessageBox::critical(this, "Error", "No vectors selected. Check the vectors you want to plot.");
        return;
    }

    plotButton->setText("Loading plot...");
    update();
    QApplication::processEvents();

    QString filename = (field("savePlot").toBool() ? field("plotFilename").toString() : "/tmp/gnuout");
    if (filename.isEmpty()) filename = "/tmp/gnuout";
    engine->plotResults(toPlot, field("plotFormat").toString() == "PNG", filename);

    plotButton->setText("Plot");
}

void SimulateWizardPage::initData()
{
    resultsLabel->setText(engine->getPlotInfo() + "\n\nVectors:");
    vectors = engine->vectors();
    for (int i = 0; i < vectors.length(); i++) {
        if (vectors[i] == "time") continue;
        QCheckBox *vectorCheckbox = new QCheckBox("    " + vectors[i], this);

        connect(vectorCheckbox, &QCheckBox::stateChanged, [=](int state){
            selectedVectors[vectors[i]] = (state == Qt::Checked);
        });

        layout->addWidget(vectorCheckbox, i);
    }

    QTabWidget *tabs = new QTabWidget(this);
    tabs->addTab(getPlotWidget(), "Plot");
    tabs->addTab(getSaveWidget(), "Save");

    layout->addWidget(tabs);
}

void SimulateWizardPage::initializePage()
{
    QString filename;
    if (field("loadCircuit").toBool()) {
        filename = field("filename").toString();
    } else {
        filename = netlist->getFilename();
    }
    defaultFilename = filename.left(filename.length() - 4);
    dumpFilenameLineEdit->setText(defaultFilename + "_dump.txt");
}

// ========== PROTECTED OVERRIDE ===========

bool SimulateWizardPage::isComplete() const
{
    if (!running) return true;
    return (progressBar->value() == 100);
}

bool SimulateWizardPage::validatePage()
{
    if (isComplete())
        engine->stopSimulation();
    return isComplete();
}

// ============ SLOTS ==================
void SimulateWizardPage::updateStatus(int progress)
{
    progressBar->setValue(progress);
    if (progress == 100) showResults();
    emit completeChanged();
}

void SimulateWizardPage::receiveError(char *errormsg){
    QMessageBox *box = new QMessageBox(QMessageBox::Critical, "Spice Error", errormsg);
    box->exec();
    delete box;
}
