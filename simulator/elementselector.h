#ifndef ELEMENTSELECTOR_H
#define ELEMENTSELECTOR_H

#include <QtWidgets>
#include "schematic.h"

class ElementSelector : public QWidget
{
    Q_OBJECT
public:
    explicit ElementSelector(Schematic *schematic, QWidget *parent = nullptr);
    void addButton(const QString &buttonLabel,
                   const QString &imgPath,
                   const QString &imgSelectedPath,
                   const QString &imgShadowPath,
                   bool hasLabel,
                   bool allowsExternalInput,
                   QString prefix,
                   QString units,
                   qreal widthRatio);
    //QString getElementPath();
    //QString getElementName();

protected:
    void mousePressEvent(QMouseEvent *) { deselectAll(); }

private:
    QGridLayout *layout;
    QButtonGroup *buttons;
    Schematic *schematic;
    QMap<int, CircuitElement::ElementProperties> properties;
    QMap<int, QString> shadowImagePaths;
    int curId = 1;
    bool deselectOnRelease;
    const qreal elementWidth = 160.0;

    void deselectAll();

public slots:
    void slotButtonPressed(int id);
    void slotButtonReleased(int id);
    void slotSchematicClicked() { deselectAll(); }
};

#endif // ELEMENTSELECTOR_H
