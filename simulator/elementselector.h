#ifndef ELEMENTSELECTOR_H
#define ELEMENTSELECTOR_H

#include <QWidget>
#include <QButtonGroup>
#include <QMap>
#include <QString>
#include <QPushButton>
#include <QApplication>

class ElementSelector : public QWidget
{
    Q_OBJECT
public:
    explicit ElementSelector(int x, int y, int width, int height, QWidget *parent = nullptr);
    void addButton(const QString &elemType,
            const QString &imgPath,
            int x, int y, int width, int height);
    QString getElementPath();
    QString getElementName();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
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
