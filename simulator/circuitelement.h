#ifndef CIRCUITELEMENT_H
#define CIRCUITELEMENT_H

#include <QApplication>
#include <QtWidgets>

#include "node.h"

class CircuitElement : public SchematicItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    struct ElementProperties
    {
        QPixmap image;
        QPixmap selected;
        bool hasLabel;
        bool allowsExternalInput;
        QString subtype;
        QString prefix;
        QString units;
        qreal widthRatio;
    };

    explicit CircuitElement(
        const CircuitElement::ElementProperties properties,
        QGraphicsItem *parent = nullptr
    );

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    int width() { return _width; }
    int height() { return _height; }

    void setNodes(Node *nodeOne, Node *nodeTwo);
    Node *getNodeOne() { return nodeOne; }
    Node *getNodeTwo() { return nodeTwo; }
    Node *getOtherNode(Node *node);

    // for adding to a netlist
    QString getName() { return prefix + name; }
    QString getValue() { return value + unitMod; }
    QString getExternalFile() { return externalFile; }

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

private:
    // GraphicsItems
    int _width, _height;
    QPixmap display, normal, selected;
    QGraphicsSimpleTextItem *label;
    Node *nodeOne;
    Node *nodeTwo;

    // Store parameters
    QString prefix;
    QString name;
    QString value;
    QString unitMod;
    QString units;
    QString externalFile;

    // Dialog box
    QDialog *dialogBox;
    QLineEdit *nameLineEdit;
    QLineEdit *valueLineEdit;
    QLineEdit *valueFileLineEdit;
    QComboBox *unitsComboBox;
    QList<QString> unitModifiers;
    QWidget *constValueExt;
    QWidget *extValueExt;

    // Private functions
    QDialog *createDialogBox(QString prefix, QString units, bool allowsExternalInput);
    void setupDialog();
    void processDialogInput();
};

#endif // CIRCUITELEMENT_H
