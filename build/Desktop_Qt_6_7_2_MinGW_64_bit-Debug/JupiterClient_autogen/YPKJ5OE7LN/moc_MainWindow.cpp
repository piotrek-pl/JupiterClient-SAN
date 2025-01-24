/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/ui/MainWindow.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSMainWindowENDCLASS = QtMocHelpers::stringData(
    "MainWindow",
    "setCurrentUsername",
    "",
    "username",
    "onStatusChanged",
    "index",
    "onRefreshFriendsListClicked",
    "onMenuSettingsTriggered",
    "onMenuExitTriggered",
    "onMenuAboutTriggered",
    "onMenuSearchTriggered",
    "onConnectionStatusChanged",
    "status",
    "onMessageReceived",
    "message",
    "onNetworkError",
    "error",
    "onDisconnected",
    "onChatWindowClosed",
    "friendId"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMainWindowENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   86,    2, 0x0a,    1 /* Public */,
       4,    1,   89,    2, 0x08,    3 /* Private */,
       6,    0,   92,    2, 0x08,    5 /* Private */,
       7,    0,   93,    2, 0x08,    6 /* Private */,
       8,    0,   94,    2, 0x08,    7 /* Private */,
       9,    0,   95,    2, 0x08,    8 /* Private */,
      10,    0,   96,    2, 0x08,    9 /* Private */,
      11,    1,   97,    2, 0x08,   10 /* Private */,
      13,    1,  100,    2, 0x08,   12 /* Private */,
      15,    1,  103,    2, 0x08,   14 /* Private */,
      17,    0,  106,    2, 0x08,   16 /* Private */,
      18,    1,  107,    2, 0x08,   17 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void, QMetaType::QJsonObject,   14,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   19,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSMainWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMainWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMainWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'setCurrentUsername'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onStatusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onRefreshFriendsListClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onMenuSettingsTriggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onMenuExitTriggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onMenuAboutTriggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onMenuSearchTriggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onConnectionStatusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onMessageReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonObject &, std::false_type>,
        // method 'onNetworkError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onDisconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onChatWindowClosed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->setCurrentUsername((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->onStatusChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->onRefreshFriendsListClicked(); break;
        case 3: _t->onMenuSettingsTriggered(); break;
        case 4: _t->onMenuExitTriggered(); break;
        case 5: _t->onMenuAboutTriggered(); break;
        case 6: _t->onMenuSearchTriggered(); break;
        case 7: _t->onConnectionStatusChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->onMessageReceived((*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 9: _t->onNetworkError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 10: _t->onDisconnected(); break;
        case 11: _t->onChatWindowClosed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMainWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
