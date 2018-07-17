#ifndef SCHEMATICITEM_H
#define SCHEMATICITEM_H

#include <QtWidgets>

class SchematicItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    SchematicItem(QString type, QGraphicsItem *parent = nullptr)
        : QGraphicsItem(parent)
    {
        setData(TypeKey, type);
    }

    virtual ~SchematicItem() {}

    QString getType() { return data(TypeKey).toString(); }
    void setType(QString type) { setData(TypeKey, type); }

    const int TypeKey = 0;
};

#endif // SCHEMATICITEM_H
