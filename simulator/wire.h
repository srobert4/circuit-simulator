#ifndef WIRE_H
#define WIRE_H

#include <QObject>
#include <QWidget>

class Wire : public QWidget
{
    Q_OBJECT
public:
    explicit Wire(QPoint start, QPoint end, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    QPoint start;
    QPoint end;

signals:

public slots:
};

#endif // WIRE_H
