#ifndef CIRCUITELEMENT_H
#define CIRCUITELEMENT_H

#include <QtWidgets>

class CircuitElement : public QWidget
{
    Q_OBJECT
public:
    explicit CircuitElement(
        int x, int y, // top left pos
        int width, int height, // size of image
        int id,
        QString imagePath,
        QWidget *parent = nullptr
    );
    void getNodePosition(QPoint &left, QPoint &right);
    void setNodeIds(int node1, int node2);

protected:
    void mousePressEvent(QMouseEvent *); // nothing
    void mouseReleaseEvent(QMouseEvent *); // nothing
    void mouseDoubleClickEvent(QMouseEvent *); // -> open dialogBox

private:
    // ID numbers
    int id;
    int node1, node2;

    // Displayed labels
    QLabel *img;
    QLabel *nameLabel;
    QLabel *valueLabel;

    // Store parameters
    QString name;
    QString value;
    QString units;

    // Dialog box
    QDialog *dialogBox;
    QLineEdit *nameLineEdit;
    QLineEdit *valueLineEdit;
    QComboBox *unitsComboBox;

    // Private functions
    QDialog *createDialogBox();
    void setupDialog();
    void processDialogInput();

signals:

public slots:
};

#endif // CIRCUITELEMENT_H
