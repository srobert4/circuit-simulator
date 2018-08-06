#ifndef BOUNDARYCONDITION_H
#define BOUNDARYCONDITION_H

#include <QtWidgets>
#include <cmath>
#include <QtMath>

class BoundaryCondition : public QObject
{
    Q_OBJECT
public:
    explicit BoundaryCondition(QString filename,
                               QObject *parent = nullptr);
    double getState(double time);
    static bool checkFile(QString filename);

private:
    QMap<qreal, qreal> states;
    double period;
    double interpolate(
            QMap<qreal, qreal>::iterator low,
            QMap<qreal, qreal>::iterator high,
            double time);


signals:
    void badFile();

public slots:
};

#endif // BOUNDARYCONDITION_H
