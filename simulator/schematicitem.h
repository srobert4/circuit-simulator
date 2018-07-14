#ifndef SCHEMATICITEM_H
#define SCHEMATICITEM_H

#include <QtWidgets>

class SchematicItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    SchematicItem(int id, QString type, QGraphicsItem *parent = nullptr)
        : QGraphicsItem(parent)
    {
        setData(IDKey, id);
        setData(TypeKey, type);
    }

    virtual ~SchematicItem() {}

    int getID() { return data(IDKey).toInt(); }
    QString getType() { return data(TypeKey).toString(); }

    void setID(int id) { setData(IDKey, id); }
    void setType(QString type) { setData(TypeKey, type); }

    const int TypeKey = 0;
    const int IDKey = 1;
};

#endif // SCHEMATICITEM_H
