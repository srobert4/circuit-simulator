#ifndef NODE_H
#define NODE_H

#include <QtWidgets>

class Node : public QWidget
{
    Q_OBJECT
public:
    explicit Node(int x, int y, QWidget *parent = nullptr);

protected:
//    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *) override;
    void enterEvent(QEvent *) override;
    void leaveEvent(QEvent *) override;
};

#endif // NODE_H
