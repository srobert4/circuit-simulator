#include "boundarycondition.h"

BoundaryCondition::BoundaryCondition(QString filename,
                                     QObject *parent) : QObject(parent)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    qreal maxTime = 0;
    qreal step = 0;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList tokens = line.split("\t");
        if (tokens.length() != 2) {
            emit badFile();
            return;
        }
        qreal time = tokens[0].toDouble();
        step = time - maxTime;
        maxTime = time > maxTime ? time : maxTime;
        states[time] = tokens[1].toDouble();
    }
    this->period = maxTime + step;
}

 bool BoundaryCondition::checkFile(QString filename)
{
     QFile file(filename);
     if (!file.open(QFile::ReadOnly | QIODevice::Text)) return false;
     QTextStream in(&file);
     bool ok = true;
     while(!in.atEnd()){
         QStringList tokens = in.readLine().split("\t");
         if (tokens.length() != 2) {
             ok = false;
             break;
         }
         tokens[0].toDouble(&ok);
         if (!ok) break;
         tokens[1].toDouble(&ok);
         if (!ok) break;
     }
     file.close();
     return ok;
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
