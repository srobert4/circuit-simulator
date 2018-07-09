#ifndef CIRCUITELEMENT_H
#define CIRCUITELEMENT_H

#include <QWidget>
#include <QLabel>
#include <QVector>
#include <QPoint>
#include <QString>

class CircuitElement : public QWidget
{
    Q_OBJECT
public:
    explicit CircuitElement(QPoint *pos,
            int height,
            int width,
            QString imagePath,
            QWidget *parent = nullptr);

private:
    QLabel *img;
    QVector<QPoint> nodes;

signals:

public slots:
};

#endif // CIRCUITELEMENT_H
