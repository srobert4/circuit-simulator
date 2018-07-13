#ifndef ELEMENTSELECTOR_H
#define ELEMENTSELECTOR_H

#include <QtWidgets>
#include "schematic.h"

class ElementSelector : public QWidget
{
    Q_OBJECT
public:
    explicit ElementSelector(Schematic *schematic, QWidget *parent = nullptr);
    void addButton(
        const QString &elemType,
        const QString &imgPath,
        const QString &imgShadowPath
    );
    QString getElementPath();
    QString getElementName();

protected:
    void mousePressEvent(QMouseEvent *) { deselectAll(); }

private:
    QGridLayout *layout;
    QButtonGroup *buttons;
    Schematic *schematic;
    QMap<int, QString> imagePaths;
    QMap<int, QString> shadowImagePaths;
    QMap<int, QString> elementTypes;
    int curId = 1;
    bool deselectOnRelease;

    void deselectAll();

public slots:
    void slotButtonPressed(int id);
    void slotButtonReleased(int id);
    void slotSchematicClicked() { deselectAll(); }
};

#endif // ELEMENTSELECTOR_H
