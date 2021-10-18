#ifndef GLOBAL_H
#define GLOBAL_H

#include "mainwindow.h"
#include "gamewindow.h"

struct global
{
    static MainWindow *mainWindow;
    static GameWindow *gameWindow;
    static bool isGameStart;
    static quint16 serverPort;
    static QHostAddress serverAddr;
    static quint16 gamePort;
    static quint64 privateKey;
    static constexpr quint32 sessionConv = 0x11223344;
    static constexpr quint32 gameConv = 0x55667788;
    static constexpr qint32 gameWidth = 10;
    static constexpr qint32 gameHeight = 20;
    static constexpr qint32 externLine = 4;
    static constexpr qint32 stackSize = 16;
    static constexpr QColor stackColor{47, 79, 79};
    static constexpr QPoint homePosition = {4, 4};
};

#endif // GLOBAL_H
