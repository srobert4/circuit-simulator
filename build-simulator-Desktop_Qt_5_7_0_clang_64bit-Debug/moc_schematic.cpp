/****************************************************************************
** Meta object code from reading C++ file 'schematic.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../simulator/schematic.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'schematic.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Schematic_t {
    QByteArrayData data[7];
    char stringdata0[49];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Schematic_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Schematic_t qt_meta_stringdata_Schematic = {
    {
QT_MOC_LITERAL(0, 0, 9), // "Schematic"
QT_MOC_LITERAL(1, 10, 16), // "schematicClicked"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 4), // "Mode"
QT_MOC_LITERAL(4, 33, 4), // "Edit"
QT_MOC_LITERAL(5, 38, 5), // "Build"
QT_MOC_LITERAL(6, 44, 4) // "Draw"

    },
    "Schematic\0schematicClicked\0\0Mode\0Edit\0"
    "Build\0Draw"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Schematic[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       1,   20, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // enums: name, flags, count, data
       3, 0x0,    3,   24,

 // enum data: key, value
       4, uint(Schematic::Edit),
       5, uint(Schematic::Build),
       6, uint(Schematic::Draw),

       0        // eod
};

void Schematic::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Schematic *_t = static_cast<Schematic *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->schematicClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Schematic::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Schematic::schematicClicked)) {
                *result = 0;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Schematic::staticMetaObject = {
    { &QGraphicsScene::staticMetaObject, qt_meta_stringdata_Schematic.data,
      qt_meta_data_Schematic,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Schematic::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Schematic::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Schematic.stringdata0))
        return static_cast<void*>(const_cast< Schematic*>(this));
    return QGraphicsScene::qt_metacast(_clname);
}

int Schematic::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsScene::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void Schematic::schematicClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
