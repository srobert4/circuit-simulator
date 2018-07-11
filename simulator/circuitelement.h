#ifndef CIRCUITELEMENT_H
#define CIRCUITELEMENT_H

#include <QtWidgets>

class CircuitElement : public QWidget
{
    Q_OBJECT
public:
    explicit CircuitElement(int x,
            int y,
            int width,
            int height, int id,
            QString imagePath,
            QWidget *parent = nullptr);
    void getNodePosition(QPoint &left, QPoint &right);
    void setNodeIds(int node1, int node2);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    int id;
    QLabel *img;
    int node1, node2;
    QString name;
    QString value;
    QString units;

// Dialog box
    QDialog *dialogBox;
    QLineEdit *nameLineEdit;
    QLineEdit *valueLineEdit;
    QComboBox *unitsComboBox;

    QWidget *label;
    QLabel *nameLabel;
    QLabel *valueLabel;

    QDialog *createDialogBox();
    void setupDialog();
    void processDialogInput();

signals:

public slots:
};

#endif // CIRCUITELEMENT_H
