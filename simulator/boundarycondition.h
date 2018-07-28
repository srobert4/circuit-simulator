#ifndef BOUNDARYCONDITION_H
#define BOUNDARYCONDITION_H

#include <QObject>
#include <QMap>
#include <QFile>
#include <QTextStream>
#include <cmath>
#include <QtMath>

class BoundaryCondition : public QObject
{
    Q_OBJECT
public:
    explicit BoundaryCondition(QString filename, qreal period, QObject *parent = nullptr);
    double getState(double time);

private:
    QMap<qreal, qreal> states;
    qreal period;
    double interpolate(
            QMap<qreal, qreal>::iterator low,
            QMap<qreal, qreal>::iterator high,
            double time);


signals:
    void badFile();

public slots:
};

#endif // BOUNDARYCONDITION_H
