#include "circuitelement.h"

CircuitElement::CircuitElement(
        int x, int y,
        int width,
        int height,
        int id,
        QString imagePath,
        QWidget *parent
) : QWidget(parent)
{
//    setAttribute(Qt::WA_NoMousePropagation);
    move(x, y);
    this->id = id;

    dialogBox = createDialogBox();
    name = "element name";
    value = "xx.xx";
    units = "unit";

    img = new QLabel(this);
    QPixmap pm(imagePath);
    img->setPixmap(pm.scaled(width, height));
    nameLabel = new QLabel(name, this);
    valueLabel = new QLabel(value + units, this);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(img, 0, 0, Qt::AlignTop | Qt::AlignLeft);
    layout->addWidget(nameLabel, 1, 0, Qt::AlignCenter);
    layout->addWidget(valueLabel, 2, 0, Qt::AlignCenter);

    setLayout(layout);
    show();
}

QDialog *CircuitElement::createDialogBox()
{
    QDialog *dialog = new QDialog();
    dialog->setModal(true);

    QLabel *nameLabel = new QLabel("Element Name: ");
    nameLineEdit = new QLineEdit;

    QLabel *valueLabel = new QLabel("Value: ");
    valueLineEdit = new QLineEdit;

    QLabel *unitsLabel = new QLabel("Units: ");
    unitsComboBox = new QComboBox;
    unitsComboBox->addItems({"Ohms", "Farads"});

    QPushButton *cancelButton = new QPushButton("Cancel");
    QPushButton *doneButton  = new QPushButton("Done");

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(nameLabel, 0, 0);
    layout->addWidget(nameLineEdit, 0, 1, 1, 2);
    layout->addWidget(valueLabel, 1, 0);
    layout->addWidget(valueLineEdit, 1, 1, 1, 2);
    layout->addWidget(unitsLabel, 2, 0);
    layout->addWidget(unitsComboBox, 2, 1, 1, 2);
    layout->addWidget(cancelButton, 3, 1);
    layout->addWidget(doneButton, 3, 2);
    dialog->setLayout(layout);

    connect(cancelButton, SIGNAL(clicked(bool)),
            dialog, SLOT(reject()));
    connect(doneButton, SIGNAL(clicked(bool)),
            dialog, SLOT(accept()));

    return dialog;
}

void CircuitElement::setNodeIds(int node1, int node2)
{
    this->node1 = node1;
    this->node2 = node2;
}

void CircuitElement::getNodePosition(QPoint &left, QPoint &right)
{
    QPoint pos = this->pos() + img->pos();
    left = pos + QPoint(0, img->height() / 2);
    right = left + QPoint(img->width(), 0);
}

void CircuitElement::setupDialog()
{
    nameLineEdit->setText(name);
    valueLineEdit->setText(value);
    unitsComboBox->setCurrentText(units);
}

void CircuitElement::processDialogInput()
{
    name = nameLineEdit->text();
    value = valueLineEdit->text();
    units = unitsComboBox->currentText();

    nameLabel->setText(name);
    valueLabel->setText(value + units);
}

void CircuitElement::mousePressEvent(QMouseEvent *event)
{
    qInfo() << "Element pressed";
}

void CircuitElement::mouseReleaseEvent(QMouseEvent *event)
{
    qInfo() << "Element released";
}

void CircuitElement::mouseDoubleClickEvent(QMouseEvent *event)
{
    setupDialog();

    int ret = dialogBox->exec();
    if (ret == QDialog::Rejected) return; // TODO reset dialog

    processDialogInput();
}
