#ifndef CIRCUITELEMENT_H
#define CIRCUITELEMENT_H

#include <QApplication>
#include <QtWidgets>


#include "symbol.h"
#include "node.h"

class CircuitElement : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit CircuitElement(
        int width, int height, // size of image
        int id, int nodeOneID, int nodeTwoID, // IDs
        QString imagePath,
        QGraphicsItem *parent = nullptr
    );

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    Node *getNodeOne() { return nodeOne; }
    Node *getNodeTwo() { return nodeTwo; }
    void setNodeIds(int nodeOne, int nodeTwo);
    void getNodeIds(int &nodeOne, int&nodeTwo);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);

private:
    // ID numbers
    int id, nodeOneId, nodeTwoId;

    // GraphicsItems
    QGraphicsSimpleTextItem *label;
    Symbol *symbol;
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

signals:

public slots:
    void slotSymbolDoubleClicked();
};

#endif // CIRCUITELEMENT_H
