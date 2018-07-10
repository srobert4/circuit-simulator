#ifndef CIRCUITELEMENT_H
#define CIRCUITELEMENT_H

#include <QtWidgets>

class CircuitElement : public QWidget
{
    Q_OBJECT
public:
    explicit CircuitElement(
            int x,
            int y,
            int width,
            int height,
            QString imagePath,
            QWidget *parent = nullptr);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    QLabel *img;
    QVector<QPoint> nodes;
    QString name;
    QString value;
    QString units;

// Dialog box
    QDialog *dialogBox;
    QLineEdit *nameLineEdit;
    QLineEdit *valueLineEdit;
    QComboBox *unitsComboBox;

    QDialog *createDialogBox();

signals:

public slots:
};

#endif // CIRCUITELEMENT_H
