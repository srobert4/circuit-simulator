#ifndef CIRCUITELEMENT_H
#define CIRCUITELEMENT_H

#include <QApplication>
#include <QtWidgets>

#include "symbol.h"
#include "node.h"

class CircuitElement : public SchematicItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit CircuitElement(
        int id, int nodeOneID, int nodeTwoID, // IDs
        const QPixmap image, const QPixmap selectedImage,
        QGraphicsItem *parent = nullptr
    );

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    Node *getNodeOne() { return nodeOne; }
    Node *getNodeTwo() { return nodeTwo; }
    void setNodeIds(int nodeOne, int nodeTwo);
    void getNodeIds(int &nodeOne, int&nodeTwo);

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

private:
    // ID numbers
    int nodeOneId, nodeTwoId;

    // GraphicsItems
    int width, height;
    QPixmap display, normal, selected;
    QGraphicsSimpleTextItem *label;
    Node *nodeOne;
    Node *nodeTwo;

    // Store parameters
    QString name;
    QString value;
    QString units;

    // Dialog box
    QDialog *dialogBox;
    QLineEdit *nameLineEdit;
    QLineEdit *valueLineEdit;
    QComboBox *unitsComboBox;

    // Private functions
    QDialog *createDialogBox();
    void setupDialog();
    void processDialogInput();
};

#endif // CIRCUITELEMENT_H
