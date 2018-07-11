#ifndef ELEMENTSELECTOR_H
#define ELEMENTSELECTOR_H

#include <QtWidgets>

class ElementSelector : public QWidget
{
    Q_OBJECT
public:
    explicit ElementSelector(QWidget *parent = nullptr);
    void addButton(
        const QString &elemType,
        const QString &imgPath
    );
    QString getElementPath();
    QString getElementName();

protected:
    void mousePressEvent(QMouseEvent *) override; // -> clickedAway()

private:
    QGridLayout *layout;
    QButtonGroup *buttons;
    QMap<int, QString> imagePaths;
    QMap<int, QString> elementTypes;
    int curId = 1;

signals:
    void clickedAway(); // -> slotDeselect()

public slots:
    void slotDeselect(); // <- clickedAway()
};

#endif // ELEMENTSELECTOR_H
