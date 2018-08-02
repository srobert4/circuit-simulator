#include "boundarycondition.h"

BoundaryCondition::BoundaryCondition(QString filename,
                                     qreal period,
                                     QObject *parent) : QObject(parent)
{
    this->period = period;
    qInfo() << "filename in boundarycondition.cpp: " << filename;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList tokens = line.split("\t");
        if (tokens.length() != 2) {
            emit badFile();
            return;
        }
        states[tokens[0].toInt()] = tokens[1].toInt();
    }
}

double BoundaryCondition::getState(double time)
{
    QMap<qreal, qreal>::iterator high, low;
    time = fmod(time, period);
    high = states.lowerBound(time);
    low = (high == states.begin() ? high : high--);

    return interpolate(low, high, time);
}

double BoundaryCondition::interpolate(
        QMap<qreal, qreal>::iterator low,
        QMap<qreal, qreal>::iterator high,
        double time)
{
    if (qFabs(high.key() - time) <= 1e-8)
        return high.value();
    if (qFabs(low.key() - time) <= 1e-8)
        return low.value();
    return low.value() + (time - low.key())*
            (high.value() - low.value())/(high.key() - low.key());
}
