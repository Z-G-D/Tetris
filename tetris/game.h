//
// Created by 666 on 2021/4/8.
//

#ifndef TETRIS_GAME_H
#define TETRIS_GAME_H

#include <QPainter>
#include <QUdpSocket>
#include <QTimer>
#include <QtCore/QQueue>
#include <ikcp.h>
#include <tetris.pb.h>
#include <QRandomGenerator>


class Player
{
public:
    explicit Player(qint32 seed);

    void clockwiseRotation();

    void counterClockwiseRotation();

    void left();

    void right();

    void softDrop();

    void hardDrop();

    void swapBlock();

    void stackClear();


    qint32 getHeightFromBottom() const;

    const QVector<QPoint> &getBlock(qint32 i) const;

    const QVector<QPoint> &getHoldBlock() const;

    const QQueue<int> &getBagOf7() const;

    const QPoint &getPosition() const;

    const QColor &getColor(qint32 i) const;

    const QColor &getHoldColor() const;

    const QVector<QVector<QPair<bool, QColor>>> &getGameMap() const;

    qint32 getStackCounter() const;

    void addStackCounter(qint32 i);

    qint32 getDamageCounter() const;

    void addDamageCounter(qint32 i);

    bool isGamOver() const;

private:
    bool isGameOver;
    QQueue<qint32> bagOf7;
    QVector<QVector<QPair<bool, QColor>>> gameMap;
    QPoint position;
    qint32 blockDirection;
    qint32 blockType;
    qint32 holdBlockType;
    qint32 stackCounter;
    qint32 damageCounter;
    QRandomGenerator randomGenerator;

    bool collisionDetection(QPoint dPoint) const;

    static const QColor color[7/*方块类型 IOTSZJL */];
    static const QVector<QPoint> block[7/*方块类型 IOTSZJL */][4/*四种方向 0,R,2,L*/];
    static const QPoint wallKickDataOfOJLSTZ[4/*四个初始方向*/][2/*两个旋转方向*/][5/*四个Test*/];
    static const QPoint wallKickDataOfI[4/*四个初始方向*/][2/*两个旋转方向*/][5/*四个Test*/];
};

class Game : public QObject
{
Q_OBJECT
public:
    Game();

    const QVector<Player> &getPlayers() const;

    qint32 getPlayerNumber() const;

    ikcpcb *getKcpClient();

    void gameOver() const;

private:
    QVector<Player> players;
    qint32 playerNumber;
    static QUdpSocket *udpSocket;

    QTimer *stackClearTimer;
    QTimer *lockDelayTimer;
    QTimer *autoFallTimer;
    QTimer *kcpTimer;
    ikcpcb *kcpClient;

    static int udp_output(const char *buf, int len, ikcpcb *kcp, void *user);

    quint32 GetAlivePlayerCount();

private slots:

    void onSocketReadyRead();

    void netSlap();

    void lockDelay();

    void stackClear();

    void autoFall();


};

#endif //TETRIS_GAME_H
