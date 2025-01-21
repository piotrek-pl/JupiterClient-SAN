/****************************************************************************
** Meta object code from reading C++ file 'NetworkManager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/network/NetworkManager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NetworkManager.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSNetworkManagerENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSNetworkManagerENDCLASS = QtMocHelpers::stringData(
    "NetworkManager",
    "connected",
    "",
    "disconnected",
    "messageReceived",
    "message",
    "loginSuccessful",
    "registrationSuccessful",
    "error",
    "connectionStatusChanged",
    "status",
    "onConnected",
    "onDisconnected",
    "onReadyRead",
    "onError",
    "QTcpSocket::SocketError",
    "socketError"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSNetworkManagerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   80,    2, 0x06,    1 /* Public */,
       3,    0,   81,    2, 0x06,    2 /* Public */,
       4,    1,   82,    2, 0x06,    3 /* Public */,
       6,    0,   85,    2, 0x06,    5 /* Public */,
       7,    0,   86,    2, 0x06,    6 /* Public */,
       8,    1,   87,    2, 0x06,    7 /* Public */,
       9,    1,   90,    2, 0x06,    9 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      11,    0,   93,    2, 0x08,   11 /* Private */,
      12,    0,   94,    2, 0x08,   12 /* Private */,
      13,    0,   95,    2, 0x08,   13 /* Private */,
      14,    1,   96,    2, 0x08,   14 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QJsonObject,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QString,   10,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 15,   16,

       0        // eod
};

Q_CONSTINIT const QMetaObject NetworkManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSNetworkManagerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSNetworkManagerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSNetworkManagerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<NetworkManager, std::true_type>,
        // method 'connected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'disconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'messageReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonObject &, std::false_type>,
        // method 'loginSuccessful'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'registrationSuccessful'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'error'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'connectionStatusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDisconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onReadyRead'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTcpSocket::SocketError, std::false_type>
    >,
    nullptr
} };

void NetworkManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<NetworkManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->disconnected(); break;
        case 2: _t->messageReceived((*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 3: _t->loginSuccessful(); break;
        case 4: _t->registrationSuccessful(); break;
        case 5: _t->error((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->connectionStatusChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->onConnected(); break;
        case 8: _t->onDisconnected(); break;
        case 9: _t->onReadyRead(); break;
        case 10: _t->onError((*reinterpret_cast< std::add_pointer_t<QTcpSocket::SocketError>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::connected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::disconnected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)(const QJsonObject & );
            if (_t _q_method = &NetworkManager::messageReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::loginSuccessful; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::registrationSuccessful; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)(const QString & );
            if (_t _q_method = &NetworkManager::error; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)(const QString & );
            if (_t _q_method = &NetworkManager::connectionStatusChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
    }
}

const QMetaObject *NetworkManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NetworkManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSNetworkManagerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int NetworkManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void NetworkManager::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void NetworkManager::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void NetworkManager::messageReceived(const QJsonObject & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void NetworkManager::loginSuccessful()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void NetworkManager::registrationSuccessful()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void NetworkManager::error(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void NetworkManager::connectionStatusChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
