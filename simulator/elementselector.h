#ifndef ELEMENTSELECTOR_H
#define ELEMENTSELECTOR_H

#include <QtWidgets>

class ElementSelector : public QWidget
{
    Q_OBJECT
public:
    explicit ElementSelector(QWidget *parent = nullptr);
    void addButton(const QString &elemType,
            const QString &imgPath);
    QString getElementPath();
    QString getElementName();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QGridLayout *layout;
    QButtonGroup *buttons;
    QMap<int, QString> imagePaths;
    QMap<int, QString> elementTypes;
    int curId = 1;

signals:
    void clickedAway();

public slots:
    void slotDeselect();
};

#endif // ELEMENTSELECTOR_H
