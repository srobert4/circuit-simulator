#ifndef SCHEMATICITEM_H
#define SCHEMATICITEM_H

#include <QtWidgets>

class SchematicItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    SchematicItem(QString type, QGraphicsItem *parent = nullptr, QString subtype = "")
        : QGraphicsItem(parent)
    {
        setData(TypeKey, type);
        setData(SubtypeKey, subtype);
    }

    virtual ~SchematicItem() {}

    QString getType() { return data(TypeKey).toString(); }
    void setType(QString type) { setData(TypeKey, type); }

    QString getSubtype() { return data(SubtypeKey).toString(); }
    void setSubtype(QString subtype) { setData(SubtypeKey, subtype); }

    const int TypeKey = 0;
    const int SubtypeKey = 1;
};

#endif // SCHEMATICITEM_H
