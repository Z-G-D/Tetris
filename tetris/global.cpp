#include "global.h"

MainWindow *global::mainWindow = nullptr;
GameWindow *global::gameWindow = nullptr;
bool global::isGameStart = false;
quint16 global::serverPort = 10000;
QHostAddress global::serverAddr{"127.0.0.1"};
quint16 global::gamePort;
quint64 global::privateKey=0X112233;
constexpr quint32 global::sessionConv;
constexpr quint32 global::gameConv;
constexpr qint32 global::gameWidth;
constexpr qint32 global::gameHeight;
constexpr qint32 global::externLine;
constexpr qint32 global::stackSize;
constexpr QColor global::stackColor;
constexpr QPoint global::homePosition;