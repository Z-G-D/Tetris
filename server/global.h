#ifndef GLOBAL_H
#define GLOBAL_H


#include <QtCore/QtGlobal>

struct global
{
    static quint16 serverPort;
    static constexpr quint32 sessionConv = 0x11223344;
    static constexpr quint32 gameConv = 0x55667788;
    static quint64 privateKey;
};

#endif // GLOBAL_H
