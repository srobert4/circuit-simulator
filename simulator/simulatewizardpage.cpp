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
    resultsLayout = new QGridLayout;

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
    layout->addLayout(resultsLayout);
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
    for (int i = 0; i < vectors.length(); i++) {
        QString vector = vectors[i];
        if (vector == "time") continue;
        QCheckBox *vectorCheckbox = new QCheckBox(this);

        connect(vectorCheckbox, &QCheckBox::stateChanged, [=](int state){
            selectedVectors[vector] = (state == Qt::Checked);
        });

        resultsLayout->addWidget(vectorCheckbox, i, 1);
    }

    plotButton = new QPushButton("Plot", this);
    connect(plotButton, &QPushButton::released, this, &SimulateWizardPage::plot);
    saveButton = new QPushButton("Save", this);
    connect(saveButton, &QPushButton::released, this, &SimulateWizardPage::save);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(plotButton);
    buttonLayout->addWidget(saveButton);
    buttonLayout->addStretch(1);
    layout->addLayout(buttonLayout);
}

void SimulateWizardPage::save()
{
    // Get filename
    bool ok;
    QString filename = defaultFilename + "_out.raw";
    QString text = QInputDialog::getText(this, "Save as",
                                         "Save as:", QLineEdit::Normal,
                                         filename, &ok);
    if (ok && !text.isEmpty())
        filename = text;

     // Get format
    bool bin = true;
    QStringList formats;
    formats << "Binary" << "ASCII";
    QString format = QInputDialog::getItem(this, tr("Output Format"),
                                           tr("Format:"), formats, 0, false, &ok);
    if (ok && !format.isEmpty())
        bin = format == "PNG";

    saveButton->setText("Saving...");
    update();
    QApplication::processEvents();
    QList<QString> toWrite;
    for (int i = 0; i < selectedVectors.size(); i++) {
        if (selectedVectors.values()[i])
            toWrite.append(selectedVectors.keys()[i]);
    }
    engine->saveResults(toWrite, bin, filename);
    QMessageBox::information(this, "Save successful", "Your results have been saved!");
    saveButton->setText("Save");
}


void SimulateWizardPage::plot()
{
    int ret = QMessageBox::question(this,
                                    "Save?",
                                    "Save this plot? \nSaving this plot will save three files: "
                                    "a .plt file containing gnuplot commands, a"
                                    " .data file containing the raw data and a .eps OR .png"
                                    " file containing the plot.",
                                    (QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel));
    if (ret == QMessageBox::Cancel) return;
    bool pngOut;
    QString filename;
    if (ret == QMessageBox::Yes) {
        // Get filename
        bool ok;
        QString text = QInputDialog::getText(this, "Save as",
                                             "Save as:", QLineEdit::Normal,
                                             defaultFilename, &ok);
        if (ok && !text.isEmpty())
            filename = text;

         // Get format
        QStringList formats;
        formats << "Postscript" << "PNG";
        QString format = QInputDialog::getItem(this, tr("Output Format"),
                                                tr("Format:"), formats, 0, false, &ok);
           if (ok && !format.isEmpty())
               pngOut = format == "PNG";
    }

    plotButton->setText("Loading plot...");
    update();
    QApplication::processEvents();
    QList<QString> toPlot;
    for (int i = 0; i < selectedVectors.size(); i++) {
        if (selectedVectors.values()[i])
            toPlot.append(selectedVectors.keys()[i]);
    }
    filename = (ret == QMessageBox::Yes ? defaultFilename : "/tmp/gnuout");
    engine->plotResults(toPlot, pngOut, filename);
    plotButton->setText("Plot");
}

void SimulateWizardPage::initData()
{
    resultsLabel->setText(engine->getPlotInfo() + "\n\nVectors:\n");
    vectors = engine->vectors();
    for (int i = 0; i < vectors.size(); i++) {
        resultsLayout->addWidget(new QLabel("    " + vectors[i], this), i, 0);
    }
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
