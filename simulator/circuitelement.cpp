#include "circuitelement.h"

CircuitElement::CircuitElement(
        int x, int y,
        int width,
        int height,
        QString imagePath,
        QWidget *parent
) : QWidget(parent)
{
    setAttribute(Qt::WA_NoMousePropagation);
    setGeometry(x, y, width, height);
    img = new QLabel(this);
    QPixmap pm(imagePath);
    img->setPixmap(pm.scaled(width, height));
    img->setGeometry(0, 0, width, height);
    img->show();

    nodes.append(QPoint(x - width / 2, y));
    nodes.append(QPoint(x + width / 2, y));

    dialogBox = createDialogBox();

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
    int ret = dialogBox->exec();
    if (ret == QDialog::Rejected) return;
    name = nameLineEdit->text();
    value = valueLineEdit->text();
    units = unitsComboBox->currentText();
}
